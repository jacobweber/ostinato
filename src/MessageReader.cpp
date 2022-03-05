#include <juce_audio_utils/juce_audio_utils.h>

#include "MessageReader.h"
#include "State.h"
#include "Step.h"
#include "Constants.h"

MessageReader::MessageReader(State &_state) : state(_state) {
}

void MessageReader::timerCallback() {
    if (state.recordedStepsFromAudioThread.try_dequeue(recordedSteps)) {
        updateSteps(recordedSteps);
    }
    if (state.updatedStepFromAudioThread.try_dequeue(updatedStepSettings)) {
        updateSteps(updatedStepSettings);
    }
}

void MessageReader::updateSteps(const RecordedSteps &steps) {
    DBG("get refresh state with " << steps.numSteps << " steps and " << steps.numVoices << " voices");

    state.voicesParameter->beginChangeGesture();
    *(state.voicesParameter) = steps.numVoices - 1;
    state.voicesParameter->endChangeGesture();

    state.stepsParameter->beginChangeGesture();
    *(state.stepsParameter) = steps.numSteps - 1;
    state.stepsParameter->endChangeGesture();

    for (size_t stepNum = 0; stepNum < static_cast<size_t>(steps.numSteps); stepNum++) {
        StepState& step = state.stepState[stepNum];
        for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(steps.numVoices); voiceNum++) {
            auto param = step.voiceParameters[voiceNum];
            param->beginChangeGesture();
            *param = steps.steps[stepNum].voices[voiceNum];
            param->endChangeGesture();
        }


        step.powerParameter->beginChangeGesture();
        *(step.powerParameter) = true;
        step.powerParameter->endChangeGesture();

        step.tieParameter->beginChangeGesture();
        *(step.tieParameter) = false;
        step.tieParameter->endChangeGesture();

        step.octaveParameter->beginChangeGesture();
        *(step.octaveParameter) = constants::MAX_OCTAVES;
        step.octaveParameter->endChangeGesture();

        step.volParameter->beginChangeGesture();
        *(step.volParameter) = steps.steps[stepNum].volume;
        step.volParameter->endChangeGesture();

        step.lengthParameter->beginChangeGesture();
        *(step.lengthParameter) = steps.steps[stepNum].length;
        step.lengthParameter->endChangeGesture();
    }
}

void MessageReader::updateSteps(const UpdatedStepSettings &stepSettings) {
    DBG("get updated state for step " << stepSettings.stepNum);

    int numVoices = state.voicesParameter->getIndex() + 1;

    StepState& step = state.stepState[static_cast<size_t>(stepSettings.stepNum)];
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(numVoices); voiceNum++) {
        auto param = step.voiceParameters[voiceNum];
        param->beginChangeGesture();
        *param = stepSettings.step.voices[voiceNum];
        param->endChangeGesture();
    }

    step.powerParameter->beginChangeGesture();
    *(step.powerParameter) = stepSettings.step.power;
    step.powerParameter->endChangeGesture();

    step.tieParameter->beginChangeGesture();
    *(step.tieParameter) = stepSettings.step.tie;
    step.tieParameter->endChangeGesture();

    step.octaveParameter->beginChangeGesture();
    *(step.octaveParameter) = stepSettings.step.octave;
    step.octaveParameter->endChangeGesture();

    step.volParameter->beginChangeGesture();
    *(step.volParameter) = stepSettings.step.vol;
    step.volParameter->endChangeGesture();

    step.lengthParameter->beginChangeGesture();
    *(step.lengthParameter) = stepSettings.step.length;
    step.lengthParameter->endChangeGesture();
}
