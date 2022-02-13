#include "StepState.h"
#include "Constants.h"

StepState& StepState::operator=(const StepState& otherStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        *(voiceParameters[voiceNum]) = otherStep.voiceParameters[voiceNum]->get();
    }
    *(octaveParameter) = otherStep.octaveParameter->getIndex();
    *(lengthParameter) = otherStep.lengthParameter->get();
    *(tieParameter) = otherStep.tieParameter->get();
    *(volParameter) = otherStep.volParameter->get();
    *(powerParameter) = otherStep.powerParameter->get();
    return *this;
}

StepState& StepState::operator=(const StepState::Temp& otherStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        *(voiceParameters[voiceNum]) = otherStep.voiceParameters[voiceNum];
    }
    *(octaveParameter) = otherStep.octaveParameter;
    *(lengthParameter) = otherStep.lengthParameter;
    *(tieParameter) = otherStep.tieParameter;
    *(volParameter) = otherStep.volParameter;
    *(powerParameter) = otherStep.powerParameter;
    return *this;
}

void StepState::toTemp(StepState::Temp& outStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        outStep.voiceParameters[voiceNum] = voiceParameters[voiceNum]->get();
    }
    outStep.octaveParameter = octaveParameter->getIndex();
    outStep.lengthParameter = lengthParameter->get();
    outStep.tieParameter = tieParameter->get();
    outStep.volParameter = volParameter->get();
    outStep.powerParameter = powerParameter->get();
}
