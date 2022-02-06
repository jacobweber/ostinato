#include <juce_audio_utils/juce_audio_utils.h>

#include "MessageReader.h"
#include "State.h"
#include "Step.h"
#include "Constants.h"

MessageReader::MessageReader(State &_state) : state(_state) {
}

void MessageReader::timerCallback() {
    if (state.updateStepsFromAudioThread.try_dequeue(updatedSteps)) {
        updateSteps(updatedSteps);
    }
}

void MessageReader::updateSteps(const UpdatedSteps &steps) {
    DBG("get refresh state with " << steps.numSteps << " steps and " << steps.numVoices << " voices");
    *(state.voicesParameter) = static_cast<int>(steps.numVoices) - 1;
    *(state.stepsParameter) = static_cast<int>(steps.numSteps) - 1;
    for (stepnum_t stepNum = 0; stepNum < steps.numSteps; stepNum++) {
        for (voicenum_t voiceNum = 0; voiceNum < steps.numVoices; voiceNum++) {
            *(state.stepState[stepNum].voiceParameters[voiceNum]) = steps.steps[stepNum].voices[voiceNum];
        }
        *(state.stepState[stepNum].powerParameter) = true;
        *(state.stepState[stepNum].tieParameter) = false;
        *(state.stepState[stepNum].octaveParameter) = constants::MAX_OCTAVES;
        *(state.stepState[stepNum].volParameter) = steps.steps[stepNum].volume;
        *(state.stepState[stepNum].lengthParameter) = steps.steps[stepNum].length;
    }
}
