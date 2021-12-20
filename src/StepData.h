#pragma once

#include "Constants.h"

class StepData {
public:
    juce::AudioParameterBool *voiceParameters[MAX_VOICES];
    juce::AudioParameterFloat *lengthParameter = nullptr;
};
