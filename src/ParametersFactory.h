#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"

// Also update State, PluginEditor/Header, test/MidiProcessorTester, test/Presets
class ParametersFactory {
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout create() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        juce::StringArray stepsChoices;
        for (stepnum_t i = 1; i <= constants::MAX_STEPS; i++) {
            stepsChoices.add(std::to_string(i));
        }
        layout.add(std::make_unique<juce::AudioParameterChoice>("steps", "Number of Steps", stepsChoices, 3));

        juce::StringArray voicesChoices;
        for (voicenum_t i = 1; i <= constants::MAX_VOICES; i++) {
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
        layout.add(std::make_unique<juce::AudioParameterChoice>("rate", "Step Length", rateChoices, 3));

        juce::StringArray rateTypeChoices;
        rateTypeChoices.add("Straight");
        rateTypeChoices.add("Triplet");
        rateTypeChoices.add("Dotted");
        layout.add(std::make_unique<juce::AudioParameterChoice>("rateType", "Step Length Type", rateTypeChoices, 0));

        juce::StringArray modeChoices; // update constants::modeChoices
        modeChoices.add("Poly");
        modeChoices.add("Mono");
        modeChoices.add("Scale");
        modeChoices.add("Chord");
        layout.add(std::make_unique<juce::AudioParameterChoice>("mode", "Mode", modeChoices, 0));

        juce::StringArray scaleChoices;
        scaleChoices.add("Major");
        scaleChoices.add("Minor");
        scaleChoices.add("Harmonic Minor");
        scaleChoices.add("Locrian");
        scaleChoices.add("Dorian");
        scaleChoices.add("Phrygian");
        scaleChoices.add("Lydian");
        scaleChoices.add("Mixolydian");
        layout.add(std::make_unique<juce::AudioParameterChoice>("scale", "Scale", scaleChoices, 0));

        juce::StringArray keyChoices;
        keyChoices.add("C");
        keyChoices.add("C#");
        keyChoices.add("D");
        keyChoices.add("D#");
        keyChoices.add("E");
        keyChoices.add("F");
        keyChoices.add("F#");
        keyChoices.add("G");
        keyChoices.add("G#");
        keyChoices.add("A");
        keyChoices.add("A#");
        keyChoices.add("B");
        layout.add(std::make_unique<juce::AudioParameterChoice>("key", "Key", keyChoices, 0));

        juce::StringArray voiceMatchingChoices; // update constants::voiceMatchingChoices
        voiceMatchingChoices.add("Start from bottom");
        voiceMatchingChoices.add("Use higher octaves if necessary");
        voiceMatchingChoices.add("Stretch/shrink voice pattern");
        voiceMatchingChoices.add("Stretch/shrink voice and step pattern");
        layout.add(std::make_unique<juce::AudioParameterChoice>("voiceMatching", "Voice Matching", voiceMatchingChoices, 0));

        for (stepnum_t i = 0; i < constants::MAX_STEPS; i++) {
            for (voicenum_t j = 0; j < constants::MAX_VOICES; j++)
                layout.add(
                        std::make_unique<juce::AudioParameterBool>(
                                "step" + std::to_string(i) + "_voice" + std::to_string(j),
                                "Step " + std::to_string(i + 1) + " Voice " +
                                std::to_string(j + 1) + " On", i == j && j < 4));

            juce::StringArray octaveChoices;
            for (int o = static_cast<int>(constants::MAX_OCTAVES); o >= -static_cast<int>(constants::MAX_OCTAVES); o--)
                octaveChoices.add(std::to_string(o));
            layout.add(std::make_unique<juce::AudioParameterChoice>("step" + std::to_string(i) + "_octave",
                                                                    "Step " + std::to_string(i + 1) + " Octave",
                                                                    octaveChoices,
                                                                    static_cast<int>(constants::MAX_OCTAVES))); // 0

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
