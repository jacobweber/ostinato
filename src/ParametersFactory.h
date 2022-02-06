#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class ParametersFactory {
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout create();
};
