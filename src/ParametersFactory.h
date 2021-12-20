#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"

class ParametersFactory {
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout create() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        juce::StringArray stepsChoices;
        for (size_t i = 1; i <= MAX_STEPS; i++) {
            stepsChoices.add(std::to_string(i));
        }
        layout.add(std::make_unique<juce::AudioParameterChoice>("steps", "Number of Steps", stepsChoices, 3));

        juce::StringArray voicesChoices;
        for (size_t i = 1; i <= MAX_VOICES; i++) {
            voicesChoices.add(std::to_string(i));
        }
        layout.add(std::make_unique<juce::AudioParameterChoice>("voices", "Number of Voices", voicesChoices, 3));

        juce::StringArray rateChoices;
        rateChoices.add("Whole");
        rateChoices.add("Half");
        rateChoices.add("Quarter");
        rateChoices.add("Eighth");
        rateChoices.add("Sixteenth");
        rateChoices.add("Thirty-Second");
        rateChoices.add("Sixty-Fourth");
        layout.add(std::make_unique<juce::AudioParameterChoice>("rate", "Step Length", rateChoices, 2));
        for (size_t i = 0; i < MAX_STEPS; i++) {
            for (size_t j = 0; j < MAX_VOICES; j++)
                layout.add(
                        std::make_unique<juce::AudioParameterBool>(
                                "step" + std::to_string(i) + "_voice" + std::to_string(j),
                                "Step " + std::to_string(i + 1) + " Voice " +
                                std::to_string(j + 1) + " On", false));
            layout.add(std::make_unique<juce::AudioParameterFloat>("step" + std::to_string(i) + "_length",
                                                                   "Step " + std::to_string(i + 1) + " Note Length",
                                                                   0.0f,
                                                                   1.0f, 0.5f));
        }
        return layout;
    }
};
