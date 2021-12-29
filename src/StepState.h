#pragma once

#include "Props.h"

class StepState {
public:
    juce::AudioParameterBool *voiceParameters[props::MAX_VOICES]{};
    juce::AudioParameterChoice *octaveParameter = nullptr;
    juce::AudioParameterFloat *lengthParameter = nullptr;
    juce::AudioParameterBool *tieParameter = nullptr;
    juce::AudioParameterFloat *volParameter = nullptr;
    juce::AudioParameterBool *powerParameter = nullptr;
};
