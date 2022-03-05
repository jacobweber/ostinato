#include "StepState.h"
#include "Constants.h"
#include "Step.h"

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

StepState& StepState::operator=(const StepSettings& otherStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        voiceParameters[voiceNum]->beginChangeGesture();
        *(voiceParameters[voiceNum]) = otherStep.voices[voiceNum];
        voiceParameters[voiceNum]->endChangeGesture();
    }

    octaveParameter->beginChangeGesture();
    *(octaveParameter) = otherStep.octave;
    octaveParameter->endChangeGesture();

    lengthParameter->beginChangeGesture();
    *(lengthParameter) = otherStep.length;
    lengthParameter->endChangeGesture();

    tieParameter->beginChangeGesture();
    *(tieParameter) = otherStep.tie;
    tieParameter->endChangeGesture();

    volParameter->beginChangeGesture();
    *(volParameter) = otherStep.vol;
    volParameter->endChangeGesture();

    powerParameter->beginChangeGesture();
    *(powerParameter) = otherStep.power;
    powerParameter->endChangeGesture();

    return *this;
}

void StepState::toStepSettings(StepSettings& outStep) {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(constants::MAX_VOICES); voiceNum++) {
        outStep.voices[voiceNum] = voiceParameters[voiceNum]->get();
    }
    outStep.octave = octaveParameter->getIndex();
    outStep.length = lengthParameter->get();
    outStep.tie = tieParameter->get();
    outStep.vol = volParameter->get();
    outStep.power = powerParameter->get();
}
