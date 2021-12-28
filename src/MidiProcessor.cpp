#include <math.h>

#include "MidiProcessor.h"
#include "Props.h"
#include "Step.h"

void MidiProcessor::init(double sr) {
    sampleRate = sr;

    pressedNotes.clear();
    playingNotes.clear();

    cycleOn = false;
    transportOn = false;
    prevPpqPos = 0;
    nextPpqPos = 0;
    nextStepIndex = 0;
    tieActive = false;
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
        }
    } else if (transportOn) { // stopped transport
        DBG("stopped transport");
        stopPlaying(midiOut, 0);
        cycleOn = false;
        state.playing = false;
        transportOn = false;
    }

    // make sure we're tracking ppq correctly
    double ppqPerBlock = numSamples * posInfo.bpm / 60 / sampleRate;
    if (transportOn) {
        if (posInfo.ppqPosition <= prevPpqPos && cycleOn) { // jumped back, so we may be looping
            // move scheduled release/play times back relative to their expected nextPpqPos
            releasePpqPos -= nextPpqPos - posInfo.ppqPosition;
            nextStepPpqPos -= nextPpqPos - posInfo.ppqPosition;
            DBG("looping back to " << posInfo.ppqPosition << " ppq, moved next step to " << nextStepPpqPos << " ppq");
        }

        prevPpqPos = posInfo.ppqPosition;
        nextPpqPos = posInfo.ppqPosition + ppqPerBlock;
    }

    double ppqPosPerStep = -1;

    if (!cycleOn) {
        if (pressedNotes.size() > 0) { // notes pressed, so start cycle
            DBG("start cycle at " << posInfo.ppqPosition << " ppq");
            cycleOn = true;
            nextStepIndex = 0;
            state.playing = true;
            state.stepIndex = 0;
            // we're not taking into account offset within block of pressing notes
            if (transportOn) {
                ppqPosPerStep = getPpqPosPerStep(state);
                nextStepPpqPos = roundStartPpqPos(posInfo.ppqPosition, ppqPosPerStep);
                releasePpqPos = -1;
                // don't calculate in samples, since tempo may change
                DBG("first step at " << nextStepPpqPos << " ppq, " << pressedNotes.size() << " pressed notes");
            } else {
                samplesUntilNextStep = static_cast<int>(props::PLAY_DELAY_SEC * sampleRate);
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
            state.playing = false;
        }
    }

    if (!cycleOn) return;

    while (true) {
        // see if we're releasing a step within this block
        int releaseSampleOffsetWithinBlock = -1;
        if (transportOn) {
            if (releasePpqPos != -1) {
                if (releasePpqPos < nextPpqPos) {
                    double ppqOffset = juce::jmax(releasePpqPos - posInfo.ppqPosition, 0.0);
                    releaseSampleOffsetWithinBlock = static_cast<int>(std::floor(
                            numSamples * (ppqOffset / ppqPerBlock)));
                    releasePpqPos = -1;
                }
            }
        } else {
            if (samplesUntilRelease != -1) {
                if (samplesUntilRelease < numSamples) {
                    releaseSampleOffsetWithinBlock = samplesUntilRelease;
                    samplesUntilRelease = -1;
                } else {
                    samplesUntilRelease -= numSamples;
                }
            }
        }

        if (releaseSampleOffsetWithinBlock != -1 && !tieActive) {
            // release a step within this block
            stopPlaying(midiOut, releaseSampleOffsetWithinBlock);
            DBG("release step at " << releaseSampleOffsetWithinBlock << " samples into block");
        }

        // see if we're playing a step within this block
        int playSampleOffsetWithinBlock = -1;
        if (transportOn) {
            if (nextStepPpqPos < nextPpqPos) {
                double ppqOffset = juce::jmax(nextStepPpqPos - posInfo.ppqPosition, 0.0);
                playSampleOffsetWithinBlock = static_cast<int>(std::floor(numSamples * (ppqOffset / ppqPerBlock)));
            }
        } else {
            if (samplesUntilNextStep < numSamples) {
                playSampleOffsetWithinBlock = samplesUntilNextStep;
            } else {
                samplesUntilNextStep -= numSamples;
            }
        }

        if (playSampleOffsetWithinBlock != -1) {
            // play a step within this block
            size_t numVoices = static_cast<size_t>(state.voicesParameter->getIndex()) + 1;
            size_t numSteps = static_cast<size_t>(state.stepsParameter->getIndex()) + 1;

            if (nextStepIndex >= numSteps) {
                nextStepIndex = 0;
            }
            state.stepIndex = nextStepIndex;

            currentStep.power = state.stepState[nextStepIndex].powerParameter->get();
            currentStep.volume = state.stepState[nextStepIndex].volParameter->get();
            currentStep.octave = state.stepState[nextStepIndex].octaveParameter->getIndex();
            currentStep.length = state.stepState[nextStepIndex].lengthParameter->get();
            currentStep.tie = state.stepState[nextStepIndex].tieParameter->get();
            for (size_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
                currentStep.voices[voiceNum] = state.stepState[nextStepIndex].voiceParameters[voiceNum]->get();
            }

            nextStepIndex++;
            if (nextStepIndex >= numSteps) {
                nextStepIndex = 0;
            }

            if (currentStep.power && !tieActive) {
                int transpose = (currentStep.octave - static_cast<int>(props::MAX_OCTAVES)) * 12;
                for (size_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
                    if (currentStep.voices[voiceNum]) {
                        int voiceIndex = static_cast<int>(numVoices - 1 - voiceNum); // they're flipped
                        if (voiceIndex < pressedNotes.size()) {
                            MidiValue noteValue = pressedNotes[static_cast<int>(voiceIndex)];
                            noteValue.note += transpose;
                            if (noteValue.note >= 0 && noteValue.note <= 127) {
                                double vel = juce::jmin(currentStep.volume * 2 * noteValue.vel, 127.0);
                                midiOut.addEvent(
                                        juce::MidiMessage::noteOn(noteValue.channel, noteValue.note,
                                                                  (juce::uint8) vel),
                                        playSampleOffsetWithinBlock);
                                playingNotes.add(noteValue);
                            }
                        }
                    }
                }
                DBG("play step at " << playSampleOffsetWithinBlock << " samples into block, vol "
                                    << currentStep.volume);
            } else {
                DBG("skip step at " << playSampleOffsetWithinBlock << " samples into block");
            }

            // if we start in the middle of a tie, we'll still play the first note the first time around
            bool tieWasActive = tieActive;
            tieActive = currentStep.tie;

            // prepare current release and next step
            if (ppqPosPerStep == -1) {
                ppqPosPerStep = getPpqPosPerStep(state);
            }

            if (transportOn) {
                if (currentStep.power) releasePpqPos = nextStepPpqPos + (currentStep.length * ppqPosPerStep);
                else if (tieWasActive) releasePpqPos = posInfo.ppqPosition; // and !tieActive?
                nextStepPpqPos = roundNextPpqPos(nextStepPpqPos + ppqPosPerStep, ppqPosPerStep);
                DBG("next step in " << ppqPosPerStep << " ppq at " << nextStepPpqPos << " ppq, length "
                                    << currentStep.length
                                    << " %, index " << nextStepIndex
                                    << ", " << pressedNotes.size()
                                    << " pressed notes, " << posInfo.bpm << " bpm");
            } else {
                double stepsPerSec = (posInfo.bpm / 60) / ppqPosPerStep;
                int samplesPerStep = static_cast<int>(std::ceil(sampleRate / stepsPerSec));
                if (currentStep.power)
                    samplesUntilRelease =
                            static_cast<int>(currentStep.length * samplesPerStep) + playSampleOffsetWithinBlock;
                else if (tieWasActive) samplesUntilRelease = playSampleOffsetWithinBlock; // and !tieActive?
                samplesUntilNextStep = samplesPerStep + playSampleOffsetWithinBlock;
                DBG("next step in " << ppqPosPerStep << " ppq or " << samplesUntilNextStep << " samples or "
                                    << (1 / stepsPerSec) << " secs, length " << currentStep.length << " %, index "
                                    << nextStepIndex
                                    << ", "
                                    << pressedNotes.size()
                                    << " pressed notes, " << posInfo.bpm << " bpm");
            }
        } else {
            break;
        }
    }
}

double MidiProcessor::getPpqPosPerStep(State &state) {
    static constexpr double EIGHT_THIRDS = 8.0 / 3.0;

    // step length in quarters = 4 / rate
    double denominator = std::pow(2, state.rateParameter->getIndex());
    switch (state.rateTypeParameter->getIndex()) {
        case 1: // triplet = 2/3 quarters = 4 * 2/3 / denom
            return EIGHT_THIRDS / denominator;
        case 2: // dotted = 3/2 quarters = 4 * 3/2 / denom
            return 6.0f / denominator;
        default: // straight
            return 4.0f / denominator;
    }
}

double MidiProcessor::roundStartPpqPos(double scheduledPpqPos, double ppqPosPerStep) {
    double offsetWithinStep = std::fmod(scheduledPpqPos, ppqPosPerStep);
    double prevStepPpqPos = scheduledPpqPos - offsetWithinStep;
    double nextStepPpqPos = prevStepPpqPos + ppqPosPerStep;
    return offsetWithinStep < props::START_DELAY_ALLOWANCE ? prevStepPpqPos : nextStepPpqPos;
}

double MidiProcessor::roundNextPpqPos(double scheduledPpqPos, double ppqPosPerStep) {
    double offsetWithinStep = std::fmod(scheduledPpqPos, ppqPosPerStep);
    double prevStepPpqPos = scheduledPpqPos - offsetWithinStep;
    double nextStepPpqPos = prevStepPpqPos + ppqPosPerStep;
    double nextStepDiff = nextStepPpqPos - scheduledPpqPos;
    return offsetWithinStep < nextStepDiff ? prevStepPpqPos : nextStepPpqPos;
}
