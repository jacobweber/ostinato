#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"
#include "Step.h"

class StepState {
public:
    std::array<juce::AudioParameterBool *, constants::MAX_VOICES> voiceParameters;
    juce::AudioParameterChoice *octaveParameter = nullptr;
    juce::AudioParameterFloat *lengthParameter = nullptr;
    juce::AudioParameterBool *tieParameter = nullptr;
    juce::AudioParameterFloat *volParameter = nullptr;
    juce::AudioParameterBool *powerParameter = nullptr;

	StepState& operator=(const StepState& otherStep);

	StepState& operator=(const StepSettings& otherStep);

	void toStepSettings(StepSettings& outStep);
};
