#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Props.h"

// Also update State, MidiProcessorTester, PluginEditor
class ParametersFactory {
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout create() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        juce::StringArray stepsChoices;
        for (size_t i = 1; i <= props::MAX_STEPS; i++) {
            stepsChoices.add(std::to_string(i));
        }
        layout.add(std::make_unique<juce::AudioParameterChoice>("steps", "Number of Steps", stepsChoices, 3));

        juce::StringArray voicesChoices;
        for (size_t i = 1; i <= props::MAX_VOICES; i++) {
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

        juce::StringArray rateTypeChoices;
        rateTypeChoices.add("Straight");
        rateTypeChoices.add("Triplet");
        rateTypeChoices.add("Dotted");
        layout.add(std::make_unique<juce::AudioParameterChoice>("rateType", "Step Length Type", rateTypeChoices, 0));

        for (size_t i = 0; i < props::MAX_STEPS; i++) {
            for (size_t j = 0; j < props::MAX_VOICES; j++)
                layout.add(
                        std::make_unique<juce::AudioParameterBool>(
                                "step" + std::to_string(i) + "_voice" + std::to_string(j),
                                "Step " + std::to_string(i + 1) + " Voice " +
                                std::to_string(j + 1) + " On", i == 3 - j));

            juce::StringArray octaveChoices;
            for (int o = -static_cast<int>(props::MAX_OCTAVES); o <= static_cast<int>(props::MAX_OCTAVES); o++)
                octaveChoices.add(std::to_string(o));
            layout.add(std::make_unique<juce::AudioParameterChoice>("step" + std::to_string(i) + "_octave",
                                                                    "Step " + std::to_string(i + 1) + " Octave",
                                                                    octaveChoices,
                                                                    static_cast<int>(props::MAX_OCTAVES))); // 0

            layout.add(std::make_unique<juce::AudioParameterFloat>("step" + std::to_string(i) + "_length",
                                                                   "Step " + std::to_string(i + 1) + " Note Length",
                                                                   0.0f,
                                                                   1.0f, 0.5f));
            layout.add(
                    std::make_unique<juce::AudioParameterBool>(
                            "step" + std::to_string(i) + "_tie",
                            "Step " + std::to_string(i + 1) + " Tie", false));
            layout.add(std::make_unique<juce::AudioParameterFloat>("step" + std::to_string(i) + "_volume",
                                                                   "Step " + std::to_string(i + 1) + " Volume",
                                                                   juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f,
                                                                                                  0.5f),
                                                                   0.5f));
            layout.add(
                    std::make_unique<juce::AudioParameterBool>(
                            "step" + std::to_string(i) + "_power",
                            "Step " + std::to_string(i + 1) + " Power", true));
        }
        return layout;
    }
};
