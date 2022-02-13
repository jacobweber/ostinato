#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"

class StepState {
public:
    struct Temp {
        std::array<bool, constants::MAX_VOICES> voiceParameters;
        int octaveParameter;
        float lengthParameter;
        bool tieParameter;
        float volParameter;
        bool powerParameter;
    };

    std::array<juce::AudioParameterBool *, constants::MAX_VOICES> voiceParameters;
    juce::AudioParameterChoice *octaveParameter = nullptr;
    juce::AudioParameterFloat *lengthParameter = nullptr;
    juce::AudioParameterBool *tieParameter = nullptr;
    juce::AudioParameterFloat *volParameter = nullptr;
    juce::AudioParameterBool *powerParameter = nullptr;

	StepState& operator=(const StepState& otherStep);

	StepState& operator=(const StepState::Temp& otherStep);

	void toTemp(StepState::Temp& outStep);
};
