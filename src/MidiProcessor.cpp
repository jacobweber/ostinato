#include <math.h>

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
    nextMidi.clear();
    for (const auto metadata: midi) {
        const auto msg = metadata.getMessage();
        MidiValue noteValue{msg.getNoteNumber(), msg.getChannel(), msg.getVelocity()};
        if (msg.isNoteOn()) {
            pressedNotes.add(noteValue);
        } else if (msg.isNoteOff()) {
            pressedNotes.removeValue(noteValue);
        } else {
            nextMidi.addEvent(msg, metadata.samplePosition);
        }
    }

    if (posInfo.isRecording || posInfo.isPlaying) {
        if (!transportOn) { // started transport
            DBG("started transport");
            cycleOn = false;
            transportOn = true;
            prevPpqPos = posInfo.ppqPosition;
            midi.swapWith(nextMidi);
            return; // skip a frame so we can calculate ppq per frame
        }
    } else if (transportOn) { // stopped transport
        DBG("stopped transport");
        stopPlaying(nextMidi, 0);
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
            midi.swapWith(nextMidi);
            return;
        }
    } else {
        if (pressedNotes.size() == 0) { // no notes pressed, so stop cycle
            DBG("stop cycle");
            stopPlaying(nextMidi, 0);
            cycleOn = false;
            midi.swapWith(nextMidi);
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
        // should reset nextStepIndex to 0 if no longer a step
        stopPlaying(nextMidi, sampleOffsetWithinFrame);
        size_t numVoices = static_cast<size_t>(state.voicesParameter->getIndex()) + 1;
        for (size_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
            if (state.stepData[nextStepIndex].voiceParameters[voiceNum]->get()) {
                auto noteIndex = static_cast<size_t>(juce::jmin(static_cast<int>(numVoices - 1 - voiceNum),
                                                                pressedNotes.size() -
                                                                1)); // repeat top note if we don't have enough
                MidiValue noteValue = pressedNotes[static_cast<int>(noteIndex)];
                nextMidi.addEvent(
                        juce::MidiMessage::noteOn(noteValue.channel, noteValue.note, (juce::uint8) noteValue.vel),
                        sampleOffsetWithinFrame);
                playingNotes.add(noteValue);
            }
        }

        // prepare next step
        nextStepIndex++;
        if (nextStepIndex > static_cast<size_t>(state.stepsParameter->getIndex())) {
            nextStepIndex = 0;
        }
        DBG("next step: " << nextStepIndex << ", rate: " << std::pow(2, state.rateParameter->getIndex()));

        double ppqPosPerStep = 4.0f / std::pow(2, state.rateParameter->getIndex());
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
    midi.swapWith(nextMidi);
}
