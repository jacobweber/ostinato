#include <juce_audio_utils/juce_audio_utils.h>

#include "ParametersFactory.h"
#include "Constants.h"

// Also update State, PluginEditor/Header, test/MidiProcessorTester, test/Presets
juce::AudioProcessorValueTreeState::ParameterLayout ParametersFactory::create() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::StringArray stepsChoices;
    for (int i = 1; i <= constants::MAX_STEPS; i++) {
        stepsChoices.add(std::to_string(i));
    }
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "steps", 1 }, "Number of Steps", stepsChoices, 3));

    juce::StringArray voicesChoices;
    for (int i = 1; i <= constants::MAX_VOICES; i++) {
        voicesChoices.add(std::to_string(i));
    }
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "voices", 1 }, "Number of Voices", voicesChoices, 3));

    juce::StringArray rateChoices;
    rateChoices.add("Whole");
    rateChoices.add("Half");
    rateChoices.add("Quarter");
    rateChoices.add("Eighth");
    rateChoices.add("Sixteenth");
    rateChoices.add("Thirty-Second");
    rateChoices.add("Sixty-Fourth");
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "rate", 1 }, "Step Length", rateChoices, 3));

    juce::StringArray rateTypeChoices;
    rateTypeChoices.add("Straight");
    rateTypeChoices.add("Triplet");
    rateTypeChoices.add("Dotted");
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "rateType", 1 }, "Step Length Type", rateTypeChoices, 0));

    juce::StringArray modeChoices; // update constants::modeChoices
    modeChoices.add("Poly");
    modeChoices.add("Mono");
    modeChoices.add("Scale");
    modeChoices.add("Chord");
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "mode", 1 }, "Mode", modeChoices, 0));

    juce::StringArray scaleChoices;
    scaleChoices.add("Major");
    scaleChoices.add("Minor");
    scaleChoices.add("Harmonic Minor");
    scaleChoices.add("Locrian");
    scaleChoices.add("Dorian");
    scaleChoices.add("Phrygian");
    scaleChoices.add("Lydian");
    scaleChoices.add("Mixolydian");
    scaleChoices.add("Major Pentatonic");
    scaleChoices.add("Minor Pentatonic");
    scaleChoices.add("Blues");
    scaleChoices.add("Whole Tone");
    scaleChoices.add("Whole-Half Tone");
    scaleChoices.add("Fourths");
    scaleChoices.add("Fifths");
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "scale", 1 }, "Scale", scaleChoices, 0));

    juce::StringArray chordScaleChoices;
    chordScaleChoices.add("Major");
    chordScaleChoices.add("Minor");
    chordScaleChoices.add("Harmonic Minor");
    chordScaleChoices.add("Locrian");
    chordScaleChoices.add("Dorian");
    chordScaleChoices.add("Phrygian");
    chordScaleChoices.add("Lydian");
    chordScaleChoices.add("Mixolydian");
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "chordScale", 1 }, "Chord Scale", chordScaleChoices, 0));

    juce::StringArray chordVoicingChoices;
    chordVoicingChoices.add("Triad");
    chordVoicingChoices.add("Seventh");
    chordVoicingChoices.add("Ninth");
    chordVoicingChoices.add("Eleventh");
    chordVoicingChoices.add("Sus 4");
    chordVoicingChoices.add("Sus 2");
    chordVoicingChoices.add("Mu");
    chordVoicingChoices.add("Sixth");
    chordVoicingChoices.add("Power");
    chordVoicingChoices.add("Quartal");
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "chordVoicing", 1 }, "Chord Voicing", chordVoicingChoices, 0));

    juce::StringArray keyChoices;
    keyChoices.add("Pressed Key");
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
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "key", 1 }, "Key", keyChoices, 0));

    for (int i = 0; i < constants::MAX_STEPS; i++) {
        for (int j = 0; j < constants::MAX_VOICES; j++)
            layout.add(
                    std::make_unique<juce::AudioParameterBool>(
                            juce::ParameterID{ "step" + std::to_string(i) + "_voice" + std::to_string(j), 1 },
                            "Step " + std::to_string(i + 1) + " Voice " +
                            std::to_string(j + 1) + " On", i == j && j < 4));

        juce::StringArray octaveChoices;
        for (int o = constants::MAX_OCTAVES; o >= -constants::MAX_OCTAVES; o--)
            octaveChoices.add(std::to_string(o));
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "step" + std::to_string(i) + "_octave", 1 },
                                                                "Step " + std::to_string(i + 1) + " Octave",
                                                                octaveChoices,
                                                                constants::MAX_OCTAVES)); // 0

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "step" + std::to_string(i) + "_length", 1 },
                                                                "Step " + std::to_string(i + 1) + " Note Length",
                                                                0.0f,
                                                                1.0f, 0.5f));
        layout.add(
                std::make_unique<juce::AudioParameterBool>(
                        juce::ParameterID{ "step" + std::to_string(i) + "_tie", 1 },
                        "Step " + std::to_string(i + 1) + " Tie", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "step" + std::to_string(i) + "_volume", 1 },
                                                                "Step " + std::to_string(i + 1) + " Volume",
                                                                juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f,
                                                                                                0.5f),
                                                                0.5f));
        layout.add(
                std::make_unique<juce::AudioParameterBool>(
                        juce::ParameterID{ "step" + std::to_string(i) + "_power", 1 },
                        "Step " + std::to_string(i + 1) + " Power", true));
    }
    return layout;
}
