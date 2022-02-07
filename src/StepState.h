#pragma once

#include "Constants.h"

class StepState {
public:
    std::array<juce::AudioParameterBool *, constants::MAX_VOICES> voiceParameters;
    juce::AudioParameterChoice *octaveParameter = nullptr;
    juce::AudioParameterFloat *lengthParameter = nullptr;
    juce::AudioParameterBool *tieParameter = nullptr;
    juce::AudioParameterFloat *volParameter = nullptr;
    juce::AudioParameterBool *powerParameter = nullptr;
};
