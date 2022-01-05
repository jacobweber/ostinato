#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "Step.h"

class MessageReader : public juce::Timer {
public:
    explicit MessageReader(State &_state) : state(_state) {
    }

    void timerCallback() override {
        if (state.updateStepsFromAudioThread.try_dequeue(updatedSteps)) {
            updateSteps(updatedSteps);
        }
        if (state.messagesFromAudioThread.try_dequeue(msg)) {
            switch (msg) {
                case State::MSG_SCALE_ON:
                    *(state.stretchParameter) = false;
                    break;
            }
        }
    }

    void updateSteps(const UpdatedSteps &steps) {
        DBG("get refresh state with " << steps.numSteps << " steps and " << steps.numVoices << " voices");
        *(state.voicesParameter) = static_cast<int>(steps.numVoices) - 1;
        *(state.stepsParameter) = static_cast<int>(steps.numSteps) - 1;
        for (size_t stepNum = 0; stepNum < steps.numSteps; stepNum++) {
            for (size_t voiceNum = 0; voiceNum < steps.numVoices; voiceNum++) {
                *(state.stepState[stepNum].voiceParameters[voiceNum]) = steps.steps[stepNum].voices[voiceNum];
            }
            *(state.stepState[stepNum].powerParameter) = true;
            *(state.stepState[stepNum].tieParameter) = false;
            *(state.stepState[stepNum].octaveParameter) = constants::MAX_OCTAVES;
            *(state.stepState[stepNum].volParameter) = steps.steps[stepNum].volume;
            *(state.stepState[stepNum].lengthParameter) = steps.steps[stepNum].length;
        }
    }

private:
    State &state;
    UpdatedSteps updatedSteps{};
    int msg = 0;
};
