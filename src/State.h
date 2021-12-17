#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "StripData.h"
#include "Constants.h"

class State {
public:
    State(juce::AudioProcessorValueTreeState &p) : parameters(p) {
        speedParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter("speed"));
        for (size_t i = 0; i < NUM_STEPS; i++) {
            for (size_t j = 0; j < NUM_VOICES; j++) {
                juce::AudioParameterBool *param = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                        "voice_" + std::to_string(i) + "_" + std::to_string(j)));
                stepData[i].voiceParameters[j] = param ? param : nullptr;
            }
        }
    }

public:
    juce::AudioProcessorValueTreeState &parameters;

    juce::AudioParameterFloat *speedParameter = nullptr;
    std::array<StripData, NUM_VOICES> stepData;
};
