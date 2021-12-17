#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"

class State {
public:
    State(juce::AudioProcessorValueTreeState &p) : parameters(p) {
    }

public:
    juce::AudioProcessorValueTreeState &parameters;
};
