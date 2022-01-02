#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "Props.h"
#include "MidiValue.h"
#include "Step.h"

class Recorder {
public:
    typedef std::array<juce::SortedSet<MidiValue>, props::MAX_STEPS> NotesInSteps;

    explicit Recorder(State &_state) : state(_state), recording(false) {
    }

    void prepareToPlay(double _sampleRate, int) {
        sampleRate = _sampleRate;
        maxSamplesBetweenSteps = static_cast<int>(props::PLAY_DELAY_SEC * sampleRate);
        recording = false;
    }

    bool isRecording() const {
        return recording;
    }

    void handleRecordButtonEnabled() {
        if (recording) return;
        notesInCurrentStep.clear();
        samplesUntilNextStep = -1;
        recording = true;
        numSteps = 0;
        DBG("start recording");
    }

    void handleRecordButtonDisabled() {
        if (!recording) return;
        DBG("stop recording");
        recording = false;
        finalizeStep(false);
    }

    void insertRest() {
        if (!recording) return;
        DBG("insert rest step " << numSteps);
        finalizeStep(true);
    }

    void process(int numSamples, juce::MidiBuffer &midiIn, juce::MidiBuffer &midiOut,
                 const juce::AudioPlayHead::CurrentPositionInfo &) {
        if (state.recordedRest) {
            state.recordedRest = false;
            insertRest();
        }
        int lastNoteInBlockSamplePos = 0;
        for (const auto metadata: midiIn) {
            const auto msg = metadata.getMessage();
            if (msg.isNoteOn()) {
                DBG("note " << msg.getDescription() << "  " << (metadata.samplePosition - lastNoteInBlockSamplePos)
                            << " since last in block, "
                            << samplesUntilNextStep << " until next");
                if (samplesUntilNextStep != -1) {
                    if (metadata.samplePosition - lastNoteInBlockSamplePos > samplesUntilNextStep) {
                        finalizeStep(false);
                        if (!recording) return;
                    }
                }
                lastNoteInBlockSamplePos = metadata.samplePosition;
                samplesUntilNextStep = maxSamplesBetweenSteps;
                MidiValue noteValue{msg.getNoteNumber(), msg.getChannel(), msg.getVelocity()};
                notesInCurrentStep.add(noteValue);
            }
            midiOut.addEvent(msg, metadata.samplePosition);
        }
        if (samplesUntilNextStep != -1) {
            samplesUntilNextStep = juce::jmax(0, samplesUntilNextStep - (numSamples - lastNoteInBlockSamplePos));
            if (samplesUntilNextStep == 0) {
                finalizeStep(false);
                samplesUntilNextStep = -1;
            }
        }
    }

    static void refreshParamsFromGUIThread(State &_state, const UpdatedSteps &steps) {
        DBG("get refresh state with " << steps.numSteps << " steps and " << steps.numVoices << " voices");
        *(_state.voicesParameter) = static_cast<int>(steps.numVoices) - 1;
        *(_state.stepsParameter) = static_cast<int>(steps.numSteps) - 1;
        for (size_t stepNum = 0; stepNum < steps.numSteps; stepNum++) {
            for (size_t voiceNum = 0; voiceNum < steps.numVoices; voiceNum++) {
                *(_state.stepState[stepNum].voiceParameters[voiceNum]) = steps.steps[stepNum].voices[voiceNum];
            }
            *(_state.stepState[stepNum].powerParameter) = true;
            *(_state.stepState[stepNum].tieParameter) = false;
            *(_state.stepState[stepNum].octaveParameter) = props::MAX_OCTAVES;
            *(_state.stepState[stepNum].volParameter) = steps.steps[stepNum].volume;
            *(_state.stepState[stepNum].lengthParameter) = steps.steps[stepNum].length;
        }
    }

private:
    void forceStopRecording() {
        DBG("force stop recording");
        recording = false;
        state.recordButton = false;
    }

    void finalizeStep(bool allowEmpty) {
        if (!allowEmpty && notesInCurrentStep.isEmpty()) return;
        DBG("finalize step " << numSteps);
        notesInSteps[numSteps] = notesInCurrentStep;
        numSteps++;
        notesInCurrentStep.clear();
        if (numSteps == props::MAX_STEPS) {
            forceStopRecording();
        }
        // hacky; should avoid copying
        state.updateStepsFromAudioThread.try_enqueue(getUpdatedSteps());
    }

    UpdatedSteps getUpdatedSteps() {
        UpdatedSteps steps{};
        juce::SortedSet<int> voices;
        steps.numSteps = numSteps;
        for (size_t stepNum = 0; stepNum < steps.numSteps; stepNum++) {
            DBG("step " << stepNum);
            auto const &notesInStep = notesInSteps[stepNum];
            for (auto const &midiValue: notesInStep) {
                DBG("  " << midiValue.note << ", " << midiValue.vel);
                voices.add(midiValue.note);
            }
        }

        // ignore extra notes on top
        steps.numVoices = juce::jmin(props::MAX_VOICES, static_cast<size_t>(voices.size()));

        for (size_t stepNum = 0; stepNum < steps.numSteps; stepNum++) {
            auto const &notesInStep = notesInSteps[stepNum];
            for (size_t voiceNum = 0; voiceNum < steps.numVoices; voiceNum++) {
                steps.steps[stepNum].voices[steps.numVoices - 1 - voiceNum] = false;
            }
            int totalVel = 0;
            int numActualNotes = 0;
            for (auto const &midiValue: notesInStep) {
                int index = voices.indexOf(midiValue.note);
                if (index != -1 && index < static_cast<int>(steps.numVoices)) {
                    steps.steps[stepNum].voices[steps.numVoices - 1 - static_cast<size_t>(index)] = true;
                    totalVel += midiValue.vel;
                    numActualNotes++;
                }
            }
            steps.steps[stepNum].length = .5;
            int avgVel = numActualNotes == 0 ? 64 : totalVel / numActualNotes;
            steps.steps[stepNum].volume = static_cast<float>(avgVel) / 127.0f;
        }

        /*
        juce::String grid = juce::String();
        for (size_t j = 0; j < steps.numVoices; j++) {
            for (size_t i = 0; i < steps.numSteps; i++) {
                grid += steps.steps[i].voices[j] ? '*' : '-';
            }
            grid += '\n';
        }
        DBG(grid);
        */
        return steps;
    }

private:
    State &state;
    double sampleRate = 0.0;
    int maxSamplesBetweenSteps = 0;

    bool recording = false;

    juce::SortedSet<MidiValue> notesInCurrentStep;
    int samplesUntilNextStep = -1;

    NotesInSteps notesInSteps;
    size_t numSteps = 0;

    std::atomic<bool> refreshUI = false;
};
