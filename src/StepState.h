#pragma once

#include "Constants.h"

class StepState {
public:
    juce::AudioParameterBool *voiceParameters[constants::MAX_VOICES]{};
    juce::AudioParameterChoice *octaveParameter = nullptr;
    juce::AudioParameterFloat *lengthParameter = nullptr;
    juce::AudioParameterBool *tieParameter = nullptr;
    juce::AudioParameterFloat *volParameter = nullptr;
    juce::AudioParameterBool *powerParameter = nullptr;
};
