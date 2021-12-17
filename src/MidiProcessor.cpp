#include "MidiProcessor.h"
#include "Constants.h"

void MidiProcessor::init(double sr) {
    sampleRate = sr;

    pressedNotes.clear();
    playingNotes.clear();

    cycleOn = false;
    transportOn = false;
    prevPpqPos = 0;
    nextStepIndex = 0;
}

void MidiProcessor::stopPlaying(juce::MidiBuffer &midi, int offset) {
    for (auto const &noteValue: playingNotes) {
        midi.addEvent(juce::MidiMessage::noteOff(noteValue.channel, noteValue.note), offset);
    }
    playingNotes.clear();
}

void
MidiProcessor::process(int numSamples, juce::MidiBuffer &midi,
                       const juce::AudioPlayHead::CurrentPositionInfo &posInfo, State &state) {
    for (const auto metadata: midi) {
        const auto msg = metadata.getMessage();
        MidiValue noteValue{msg.getNoteNumber(), msg.getChannel()};
        if (msg.isNoteOn()) {
            pressedNotes.add(noteValue);
        } else if (msg.isNoteOff()) {
            pressedNotes.removeValue(noteValue);
        }
    }

    midi.clear();
    if (posInfo.isRecording || posInfo.isPlaying) {
        if (!transportOn) { // started transport
            DBG("started transport");
            cycleOn = false;
            transportOn = true;
            prevPpqPos = posInfo.ppqPosition;
            return; // skip a frame so we can calculate ppq per frame
        }
    } else if (transportOn) { // stopped transport
        DBG("stopped transport");
        stopPlaying(midi, 0);
        cycleOn = false;
        transportOn = false;
    }

    if (!cycleOn) {
        if (pressedNotes.size() > 0) { // notes pressed, so start cycle
            DBG("start cycle at " << posInfo.ppqPosition);
            cycleOn = true;
            nextStepIndex = 0;
            if (transportOn) {
                // start at next beat; don't try to align to bars
                nextStepPpqPos = std::floor(posInfo.ppqPosition);
                if (posInfo.ppqPosition - nextStepPpqPos > 0.05) {
                    // Reaper seems to start a little late
                    nextStepPpqPos += 1;
                }
                // don't calculate in samples, since tempo may change
                DBG("first step at " << nextStepPpqPos << " ppq, " << pressedNotes.size()
                                     << " pressed notes");
            } else {
                samplesUntilNextStep = static_cast<int>(PLAY_DELAY_SEC * sampleRate);
                DBG("first step in " << samplesUntilNextStep << " samples, " << pressedNotes.size()
                                     << " pressed notes");
            }
        } else {
            return;
        }
    } else {
        if (pressedNotes.size() == 0) { // no notes pressed, so stop cycle
            DBG("stop cycle");
            stopPlaying(midi, 0);
            cycleOn = false;
            return;
        }
    }

    jassert(pressedNotes.size() > 0);

    // see if we're playing a step within this frame
    int sampleOffsetWithinFrame = -1;
    if (transportOn) {
        double ppqPerFrame = posInfo.ppqPosition - prevPpqPos; // changes depending on tempo/meter
        prevPpqPos = posInfo.ppqPosition;
        double frameEndPpqPos = posInfo.ppqPosition + ppqPerFrame;
        if (nextStepPpqPos < frameEndPpqPos) {
            double ppqOffset = juce::jmax(nextStepPpqPos - posInfo.ppqPosition, 0.0);
            sampleOffsetWithinFrame = static_cast<int>(std::floor(numSamples * (ppqOffset / ppqPerFrame)));
        }
    } else {
        if (samplesUntilNextStep < numSamples) {
            sampleOffsetWithinFrame = samplesUntilNextStep;
        } else {
            samplesUntilNextStep -= numSamples;
        }
    }

    if (sampleOffsetWithinFrame != -1) {
        // play a step within this frame
        size_t voiceNum = nextStepIndex % NUM_VOICES; // for now
        auto noteIndex = static_cast<size_t>(juce::jmin((int) voiceNum, pressedNotes.size() -
                                                                        1)); // repeat top note if we don't have enough
        MidiValue noteValue = pressedNotes[static_cast<int>(noteIndex)];
        stopPlaying(midi, samplesUntilNextStep);
        midi.addEvent(juce::MidiMessage::noteOn(noteValue.channel, noteValue.note, (juce::uint8) 90),
                      sampleOffsetWithinFrame);
        playingNotes.add(noteValue);

        // prepare next step
        nextStepIndex = (nextStepIndex + 1) % NUM_STEPS;
        double ppqPosPerStep = 4.0f / STEP_VALUE;
        if (transportOn) {
            nextStepPpqPos += ppqPosPerStep;
            DBG("next step in " << ppqPosPerStep << " ppq at " << nextStepPpqPos << " ppq, " << pressedNotes.size()
                                << " pressed notes, " << posInfo.bpm << " bpm");
        } else {
            double stepsPerSec = (posInfo.bpm / 60) / ppqPosPerStep;
            samplesUntilNextStep = static_cast<int>(std::ceil(sampleRate / stepsPerSec));
            DBG("next step in " << ppqPosPerStep << " ppq or " << samplesUntilNextStep << " samples or "
                                << (1 / stepsPerSec) << " secs, " << pressedNotes.size()
                                << " pressed notes, " << posInfo.bpm << " bpm");
        }
    }
}
