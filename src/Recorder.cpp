#include <juce_audio_utils/juce_audio_utils.h>

#include "Recorder.h"
#include "State.h"
#include "Constants.h"
#include "MidiValue.h"
#include "Step.h"

Recorder::Recorder(State &_state) : state(_state), recording(Inactive) {
}

void Recorder::prepareToPlay(double _sampleRate, int) {
    DBG("prepareToPlay: stop recording");
    sampleRate = _sampleRate;
    maxSamplesBetweenSteps = static_cast<int>(constants::PLAY_DELAY_SEC * sampleRate);
    recording = Inactive;
}

Recorder::Status Recorder::getStatus() const {
    return recording;
}

void Recorder::resetStatus() {
    recording = Inactive;
}

void Recorder::handleRecordButtonOn() {
    if (recording == Active) return;
    DBG("start recording");
    notesInCurrentStep.clear();
    samplesUntilStepFinalized = -1;
    recording = Active;
    numSteps = 0;
}

void Recorder::handleRecordButtonOff() {
    if (recording != Active) return;
    DBG("stop recording");
    recording = Inactive;
    finalizeStep(false);
}

void Recorder::insertRest() {
    if (recording != Active) return;
    DBG("insert rest step " << numSteps);
    finalizeStep(true);
}

void Recorder::process(int numSamples, juce::MidiBuffer &midiIn, juce::MidiBuffer &midiOut,
                const juce::AudioPlayHead::CurrentPositionInfo &) {
    if (state.recordedRest) {
        state.recordedRest = false;
        insertRest();
    }
    int lastNoteInBlockSamplePos = 0;
    for (const auto metadata: midiIn) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            DBG("note " << msg.getNoteNumber() << " at "
                        << metadata.samplePosition << " samples, last note "
                        << lastNoteInBlockSamplePos
                        << " samples, "
                        << samplesUntilStepFinalized << " until finalized");
            if (samplesUntilStepFinalized != -1) {
                if (metadata.samplePosition - lastNoteInBlockSamplePos > samplesUntilStepFinalized) {
                    finalizeStep(false);
                }
            }
            if (recording != RanOutOfSteps) {
                lastNoteInBlockSamplePos = metadata.samplePosition;
                samplesUntilStepFinalized = maxSamplesBetweenSteps;
                MidiValue noteValue{msg.getNoteNumber(), msg.getChannel(), msg.getVelocity()};
                notesInCurrentStep.add(noteValue);
            }
        }
        if (!msg.isSustainPedalOn() && !msg.isSustainPedalOff()) {
            midiOut.addEvent(msg, metadata.samplePosition);
        }
    }
    if (samplesUntilStepFinalized != -1) {
        samplesUntilStepFinalized = juce::jmax(0,
                                                samplesUntilStepFinalized - (numSamples - lastNoteInBlockSamplePos));
        if (samplesUntilStepFinalized == 0) {
            finalizeStep(false);
        }
    }
}

void Recorder::finalizeStep(bool allowEmpty) {
    samplesUntilStepFinalized = -1;
    if (!allowEmpty && notesInCurrentStep.isEmpty()) return;
    DBG("finalize step " << numSteps);
    notesInSteps[static_cast<size_t>(numSteps)] = notesInCurrentStep;
    numSteps++;
    notesInCurrentStep.clear();
    if (numSteps == constants::MAX_STEPS) {
        DBG("ran out of steps");
        recording = RanOutOfSteps;
    }
    // should avoid copying
    state.updateStepsFromAudioThread.try_enqueue(getUpdatedSteps());
}

UpdatedSteps Recorder::getUpdatedSteps() {
    UpdatedSteps steps{};
    juce::SortedSet<int> voices;
    steps.numSteps = numSteps;
    for (size_t stepNum = 0; stepNum < static_cast<size_t>(steps.numSteps); stepNum++) {
        DBG("step " << stepNum);
        auto const &notesInStep = notesInSteps[stepNum];
        for (auto const &midiValue: notesInStep) {
            DBG("  " << midiValue.note << ", " << midiValue.vel);
            voices.add(midiValue.note);
        }
    }

    // ignore extra notes on top
    steps.numVoices = juce::jmax(1, juce::jmin(constants::MAX_VOICES, voices.size()));

    for (size_t stepNum = 0; stepNum < static_cast<size_t>(steps.numSteps); stepNum++) {
        auto const &notesInStep = notesInSteps[stepNum];
        for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(steps.numVoices); voiceNum++) {
            steps.steps[stepNum].voices[voiceNum] = false;
        }
        int totalVel = 0;
        int numActualNotes = 0;
        for (auto const &midiValue: notesInStep) {
            int index = voices.indexOf(midiValue.note);
            if (index != -1 && index < steps.numVoices) {
                steps.steps[stepNum].voices[static_cast<size_t>(index)] = true;
                totalVel += midiValue.vel;
                numActualNotes++;
            }
        }
        steps.steps[stepNum].length = .5;
        int avgVel = numActualNotes == 0 ? 64 : totalVel / numActualNotes;
        steps.steps[stepNum].volume = static_cast<float>(avgVel + 1) / 128.0f;
    }

    /*
    juce::String grid = juce::String();
    for (size_t j = 0; j < static_cast<size_t>(steps.numVoices); j++) {
        for (size_t i = 0; i < static_cast<size_t>(steps.numSteps); i++) {
            grid += steps.steps[i].voices[j] ? '*' : '-';
        }
        grid += '\n';
    }
    DBG(grid);
    */
    return steps;
}
