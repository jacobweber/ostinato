#pragma once

#include "Constants.h"

class StepState {
public:
    juce::AudioParameterBool *voiceParameters[MAX_VOICES];
    juce::AudioParameterFloat *lengthParameter = nullptr;
    juce::AudioParameterBool *tieParameter = nullptr;
    juce::AudioParameterFloat *volParameter = nullptr;
    juce::AudioParameterBool *powerParameter = nullptr;
};
