#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

namespace constants {
    enum voiceMatchingChoices {
        StartFromBottom,
        UseHigherOctaves,
        StretchVoicePattern,
        StretchVoiceStepsPattern
    };
    enum randomChoices {
        Off,
        On,
        Sticky,
    };

    enum modeChoices {
        Poly,
        Mono,
        Scale,
        Chord
    };
    enum scaleChoices {
        Major,
        MinorNatural,
        MinorHarmonic,
        Locrian,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        MajPent,
        MinPent,
        Blues,
        Whole,
        WholeHalf,
        Fourth,
        Fifth
    };
    enum chordScaleChoices {
        CSMajor,
        CSMinorNatural,
        CSMinorHarmonic,
        CSLocrian,
        CSDorian,
        CSPhrygian,
        CSLydian,
        CSMixolydian
    };
    enum voicingChoices {
        Triad,
        Seventh,
        Ninth,
        Eleventh,
        Sus4,
        Sus2,
        Mu,
        Sixth,
        Power,
        Quartal
    };
    constexpr int PRESSED_KEY = 0;

    constexpr int MAX_NOTES = 24;
    constexpr int MAX_STEPS = 16;
    constexpr int MAX_VOICES = 8;
    constexpr int MAX_OCTAVES = 4;
    constexpr int DEFAULT_RANDOMNESS = 50;

    const juce::Colour COLOR_STANDARD = juce::Colours::white;
    const juce::Colour COLOR_SLIDER = juce::Colours::white;
    const juce::Colour COLOR_HIGHLIGHT = juce::Colours::white;
    const juce::Colour COLOR_BACKGROUND = juce::Colour(42, 42, 42);
    const juce::Colour COLOR_HEADER = juce::Colour(127, 127, 127);
    const juce::Colour COLOR_STRIP_ACTIVE = juce::Colour(55, 55, 55);
    const juce::Colour COLOR_STRIP_INACTIVE = juce::Colours::black;
    const juce::Colour COLOR_TOGGLE_ACTIVE = COLOR_HIGHLIGHT;
    const juce::Colour COLOR_TOGGLE_INACTIVE = juce::Colour(76, 76, 76);
    const juce::Colour COLOR_TOGGLE_ACTIVE_TEXT = COLOR_TOGGLE_INACTIVE;
    const juce::Colour COLOR_TOGGLE_INACTIVE_TEXT = COLOR_TOGGLE_ACTIVE;
    const juce::Colour COLOR_LIGHT_ACTIVE = COLOR_HIGHLIGHT;
    const juce::Colour COLOR_LIGHT_INACTIVE = COLOR_HIGHLIGHT;
    const juce::Colour COLOR_COMBO_BG = COLOR_TOGGLE_INACTIVE;
    const juce::Colour COLOR_MESSAGE_TEXT = juce::Colours::white;

    const char *const LABEL_TIE = "Tie";
    const char *const LABEL_STEPS = "Steps";
    const char *const LABEL_VOICES = "Voices";
    const char *const LABEL_RATE = "Step Length";
    const char *const LABEL_SCALE = "Scale";
    const char *const LABEL_CHORD_SCALE = "Scale";
    const char *const LABEL_CHORD_VOICING = "Voicing";
    const char *const LABEL_KEY = "Key";
    const char *const LABEL_MODE = "Mode";
    const char *const LABEL_LENGTH = "Length";
    const char *const LABEL_OCTAVE = "Octave";

    const char *const TOOLTIP_FILE = "Save and load presets";
    const char *const TOOLTIP_RECORD = "Record pattern";
    const char *const TOOLTIP_RANDOM = "Randomize settings";
    const char *const TOOLTIP_VOICES_CLEAR = "Turn off all voices for this step";
    const char *const TOOLTIP_VOICES_FILL = "Turn on all voices for this step";
    const char *const TOOLTIP_TIE = "Hold this step instead of playing the next step";
    const char *const TOOLTIP_POWER_OFF = "Disable this step";
    const char *const TOOLTIP_POWER_ON = "Enable this step";
    const char *const TOOLTIP_SETTINGS = "Additional settings";

    const char *const MSG_SCALE = "Each voice will be a scale degree, constrained to the selected scale and key.";
    const char *const MSG_CHORD = "Each voice will be a chord tone, constrained to the selected scale, key, and voicing.";
    const char *const MSG_RECORD = "Play an arpeggiation to record it. Press the space bar to enter a rest.";

    constexpr double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
    constexpr double START_DELAY_ALLOWANCE = 0.05; // if we start playing before this many ppqs after the beat, round down to the beat. Reaper seems to start a little late.
}
