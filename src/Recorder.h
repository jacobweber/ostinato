#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "Props.h"
#include "MidiValue.h"

class Recorder {
public:
    struct UpdatedStep {
        std::array<bool, props::MAX_VOICES> voices;
        float length;
        float volume;
    };

    typedef std::array<juce::SortedSet<MidiValue>, props::MAX_STEPS> NotesInSteps;
    typedef std::array<UpdatedStep, props::MAX_STEPS> UpdatedSteps;

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
        refreshState();
    }

    void refreshState() {
        UpdatedSteps steps{};
        getUpdatedSteps(steps, numVoices);
        DBG("post refresh state");
        // very hacky
        state.changeQueueFromAudio.callf([this, steps] {
            refreshParamsFromGUIThread(steps, numSteps, numVoices);
        });
    }

    void refreshParamsFromGUIThread(const UpdatedSteps &steps, size_t _numSteps, size_t _numVoices) {
        DBG("get refresh state with " << _numSteps << " steps and " << _numVoices << " voices");
        *(state.voicesParameter) = static_cast<int>(numVoices) - 1;
        *(state.stepsParameter) = static_cast<int>(numSteps) - 1;
        for (size_t stepNum = 0; stepNum < numSteps; stepNum++) {
            for (size_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
                *(state.stepState[stepNum].voiceParameters[voiceNum]) = steps[stepNum].voices[voiceNum];
            }
            *(state.stepState[stepNum].powerParameter) = true;
            *(state.stepState[stepNum].tieParameter) = false;
            *(state.stepState[stepNum].octaveParameter) = props::MAX_OCTAVES;
            *(state.stepState[stepNum].volParameter) = steps[stepNum].volume;
            *(state.stepState[stepNum].lengthParameter) = steps[stepNum].length;
        }
    }

    void getUpdatedSteps(UpdatedSteps &outSteps, size_t &outNumVoices) {
        juce::SortedSet<int> voices;
        for (size_t stepNum = 0; stepNum < numSteps; stepNum++) {
            DBG("step " << stepNum);
            auto const &notesInStep = notesInSteps[stepNum];
            for (auto const &midiValue: notesInStep) {
                DBG("  " << midiValue.note << ", " << midiValue.vel);
                voices.add(midiValue.note);
            }
        }

        // ignore extra notes on top
        outNumVoices = juce::jmin(props::MAX_VOICES, static_cast<size_t>(voices.size()));

        for (size_t stepNum = 0; stepNum < numSteps; stepNum++) {
            auto const &notesInStep = notesInSteps[stepNum];
            for (size_t voiceNum = 0; voiceNum < outNumVoices; voiceNum++) {
                outSteps[stepNum].voices[outNumVoices - 1 - voiceNum] = false;
            }
            int totalVel = 0;
            int numActualNotes = 0;
            for (auto const &midiValue: notesInStep) {
                int index = voices.indexOf(midiValue.note);
                if (index != -1 && index < static_cast<int>(outNumVoices)) {
                    outSteps[stepNum].voices[outNumVoices - 1 - static_cast<size_t>(index)] = true;
                    totalVel += midiValue.vel;
                    numActualNotes++;
                }
            }
            outSteps[stepNum].length = .5;
            int avgVel = numActualNotes == 0 ? 64 : totalVel / numActualNotes;
            outSteps[stepNum].volume = static_cast<float>(avgVel) / 127.0f;
        }

        /*
        juce::String grid = juce::String();
        for (size_t j = 0; j < outNumVoices; j++) {
            for (size_t i = 0; i < numSteps; i++) {
                grid += outSteps[i].voices[j] ? '*' : '-';
            }
            grid += '\n';
        }
        DBG(grid);
        */
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
    size_t numVoices = 0;

    std::atomic<bool> refreshUI = false;
};
