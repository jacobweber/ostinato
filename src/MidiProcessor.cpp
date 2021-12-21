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
    nextPpqPos = 0;
    looped = false;
    nextStepIndex = 0;
}

void MidiProcessor::stopPlaying(juce::MidiBuffer &midiOut, int offset) {
    for (auto const &noteValue: playingNotes) {
        midiOut.addEvent(juce::MidiMessage::noteOff(noteValue.channel, noteValue.note), offset);
    }
    playingNotes.clear();
}

void
MidiProcessor::process(int numSamples, juce::MidiBuffer &midiIn, juce::MidiBuffer &midiOut,
                       const juce::AudioPlayHead::CurrentPositionInfo &posInfo, State &state) {
    for (const auto metadata: midiIn) {
        const auto msg = metadata.getMessage();
        MidiValue noteValue{msg.getNoteNumber(), msg.getChannel(), msg.getVelocity()};
        if (msg.isNoteOn()) {
            pressedNotes.add(noteValue);
        } else if (msg.isNoteOff()) {
            pressedNotes.removeValue(noteValue);
        } else {
            midiOut.addEvent(msg, metadata.samplePosition);
        }
    }

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
        stopPlaying(midiOut, 0);
        cycleOn = false;
        transportOn = false;
    }

    // make sure we're tracking ppq correctly
    double ppqPerFrame = 0;
    if (transportOn) {
        if (posInfo.ppqPosition <= prevPpqPos) { // jumped back, so we may be looping; can't use prevPpqPos
            looped = true;
            prevPpqPos = posInfo.ppqPosition;
            return; // skip a frame so we can calculate ppq per frame
        }

        ppqPerFrame = posInfo.ppqPosition - prevPpqPos; // changes depending on tempo/meter
        prevPpqPos = posInfo.ppqPosition;

        if (looped && cycleOn) {
            // move scheduled release/play times back relative to their expected nextPpqPos
            releasePpqPos -= nextPpqPos - posInfo.ppqPosition;
            nextStepPpqPos -= nextPpqPos - posInfo.ppqPosition;
            DBG("looping back to " << posInfo.ppqPosition << " ppq, moved next step to " << nextStepPpqPos << " ppq");
        }

        nextPpqPos = posInfo.ppqPosition + ppqPerFrame;
    }
    looped = false;

    if (!cycleOn) {
        if (pressedNotes.size() > 0) { // notes pressed, so start cycle
            DBG("start cycle at " << posInfo.ppqPosition << " ppq");
            cycleOn = true;
            nextStepIndex = 0;
            // we're not taking into account offset within frame of pressing notes
            if (transportOn) {
                // start at current beat; don't try to align to bars
                nextStepPpqPos = std::floor(posInfo.ppqPosition);
                if (posInfo.ppqPosition - nextStepPpqPos > 0.05) {
                    // Reaper seems to start a little late, otherwise start at next beat
                    nextStepPpqPos += 1;
                }
                releasePpqPos = -1;
                // don't calculate in samples, since tempo may change
                DBG("first step at " << nextStepPpqPos << " ppq, " << pressedNotes.size() << " pressed notes");
            } else {
                samplesUntilNextStep = static_cast<int>(PLAY_DELAY_SEC * sampleRate);
                samplesUntilRelease = -1;
                DBG("first step in " << samplesUntilNextStep << " samples, " << pressedNotes.size()
                                     << " pressed notes");
            }
        }
    } else {
        if (pressedNotes.size() == 0) { // no notes pressed, so stop cycle
            DBG("stop cycle");
            stopPlaying(midiOut, 0);
            cycleOn = false;
        }
    }

    if (!cycleOn) return;

    while (true) {
        // see if we're releasing a step within this frame
        int releaseSampleOffsetWithinFrame = -1;
        if (transportOn) {
            if (releasePpqPos != -1) {
                if (releasePpqPos < nextPpqPos) {
                    double ppqOffset = juce::jmax(releasePpqPos - posInfo.ppqPosition, 0.0);
                    releaseSampleOffsetWithinFrame = static_cast<int>(std::floor(
                            numSamples * (ppqOffset / ppqPerFrame)));
                    releasePpqPos = -1;
                }
            }
        } else {
            if (samplesUntilRelease != -1) {
                if (samplesUntilRelease < numSamples) {
                    releaseSampleOffsetWithinFrame = samplesUntilRelease;
                    samplesUntilRelease = -1;
                } else {
                    samplesUntilRelease -= numSamples;
                }
            }
        }

        if (releaseSampleOffsetWithinFrame != -1) {
            // release a step within this frame
            stopPlaying(midiOut, releaseSampleOffsetWithinFrame);
            DBG("release step at " << releaseSampleOffsetWithinFrame << " samples into frame");
        }

        // see if we're playing a step within this frame
        int playSampleOffsetWithinFrame = -1;
        if (transportOn) {
            if (nextStepPpqPos < nextPpqPos) {
                double ppqOffset = juce::jmax(nextStepPpqPos - posInfo.ppqPosition, 0.0);
                playSampleOffsetWithinFrame = static_cast<int>(std::floor(numSamples * (ppqOffset / ppqPerFrame)));
            }
        } else {
            if (samplesUntilNextStep < numSamples) {
                playSampleOffsetWithinFrame = samplesUntilNextStep;
            } else {
                samplesUntilNextStep -= numSamples;
            }
        }

        if (playSampleOffsetWithinFrame != -1) {
            // play a step within this frame
            size_t lastStepIndex = static_cast<size_t>(state.stepsParameter->getIndex());
            if (nextStepIndex > lastStepIndex) {
                nextStepIndex = 0;
            }

            size_t numVoices = static_cast<size_t>(state.voicesParameter->getIndex()) + 1;
            for (size_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
                if (state.stepState[nextStepIndex].voiceParameters[voiceNum]->get()) {
                    auto noteIndex = static_cast<size_t>(juce::jmin(static_cast<int>(numVoices - 1 - voiceNum),
                                                                    pressedNotes.size() -
                                                                    1)); // repeat top note if we don't have enough
                    MidiValue noteValue = pressedNotes[static_cast<int>(noteIndex)];
                    midiOut.addEvent(
                            juce::MidiMessage::noteOn(noteValue.channel, noteValue.note, (juce::uint8) noteValue.vel),
                            playSampleOffsetWithinFrame);
                    playingNotes.add(noteValue);
                }
            }
            DBG("play step at " << playSampleOffsetWithinFrame << " samples into frame");
            double length = state.stepState[nextStepIndex].lengthParameter->get();

            // prepare current release and next step
            nextStepIndex++;
            if (nextStepIndex > lastStepIndex) {
                nextStepIndex = 0;
            }
            DBG("next step index: " << nextStepIndex << ", rate: 1/" << std::pow(2, state.rateParameter->getIndex())
                                    << ", length: " << length);

            double ppqPosPerStep = 4.0f / std::pow(2, state.rateParameter->getIndex());
            if (transportOn) {
                releasePpqPos = nextStepPpqPos + (length * ppqPosPerStep);
                nextStepPpqPos += ppqPosPerStep;
                DBG("next step in " << ppqPosPerStep << " ppq at " << nextStepPpqPos << " ppq, " << pressedNotes.size()
                                    << " pressed notes, " << posInfo.bpm << " bpm");
            } else {
                double stepsPerSec = (posInfo.bpm / 60) / ppqPosPerStep;
                int samplesPerStep = static_cast<int>(std::ceil(sampleRate / stepsPerSec));
                samplesUntilRelease = static_cast<int>(length * samplesPerStep) + playSampleOffsetWithinFrame;
                samplesUntilNextStep = samplesPerStep + playSampleOffsetWithinFrame;
                DBG("next step in " << ppqPosPerStep << " ppq or " << samplesUntilNextStep << " samples or "
                                    << (1 / stepsPerSec) << " secs, " << pressedNotes.size()
                                    << " pressed notes, " << posInfo.bpm << " bpm");
            }
        } else {
            break;
        }
    }
}
