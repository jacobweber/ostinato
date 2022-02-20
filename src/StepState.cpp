#include "StepState.h"
#include "Constants.h"

StepState& StepState::operator=(const StepState& otherStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        voiceParameters[voiceNum]->beginChangeGesture();
        *(voiceParameters[voiceNum]) = otherStep.voiceParameters[voiceNum]->get();
        voiceParameters[voiceNum]->endChangeGesture();
    }

    octaveParameter->beginChangeGesture();
    *(octaveParameter) = otherStep.octaveParameter->getIndex();
    octaveParameter->endChangeGesture();

    lengthParameter->beginChangeGesture();
    *(lengthParameter) = otherStep.lengthParameter->get();
    lengthParameter->endChangeGesture();

    tieParameter->beginChangeGesture();
    *(tieParameter) = otherStep.tieParameter->get();
    tieParameter->endChangeGesture();

    volParameter->beginChangeGesture();
    *(volParameter) = otherStep.volParameter->get();
    volParameter->endChangeGesture();

    powerParameter->beginChangeGesture();
    *(powerParameter) = otherStep.powerParameter->get();
    powerParameter->endChangeGesture();

    return *this;
}

StepState& StepState::operator=(const StepState::Temp& otherStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        voiceParameters[voiceNum]->beginChangeGesture();
        *(voiceParameters[voiceNum]) = otherStep.voiceParameters[voiceNum];
        voiceParameters[voiceNum]->endChangeGesture();
    }

    octaveParameter->beginChangeGesture();
    *(octaveParameter) = otherStep.octaveParameter;
    octaveParameter->endChangeGesture();

    lengthParameter->beginChangeGesture();
    *(lengthParameter) = otherStep.lengthParameter;
    lengthParameter->endChangeGesture();

    tieParameter->beginChangeGesture();
    *(tieParameter) = otherStep.tieParameter;
    tieParameter->endChangeGesture();

    volParameter->beginChangeGesture();
    *(volParameter) = otherStep.volParameter;
    volParameter->endChangeGesture();

    powerParameter->beginChangeGesture();
    *(powerParameter) = otherStep.powerParameter;
    powerParameter->endChangeGesture();

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
