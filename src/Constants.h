#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

typedef size_t voicenum_t;
typedef size_t stepnum_t;

namespace constants {
    constexpr size_t MAX_NOTES = 24;
    constexpr stepnum_t MAX_STEPS = 16;
    constexpr voicenum_t MAX_VOICES = 8;
    constexpr size_t MAX_OCTAVES = 4;

    constexpr int STRETCH_INDEX = 2;
    constexpr int SHRINK_INDEX = 3;

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

    const char *const APP_NAME = "Ostinato";

    const char *const LABEL_TIE = "Tie";
    const char *const LABEL_STEPS = "Steps";
    const char *const LABEL_VOICES = "Voices";
    const char *const LABEL_RATE = "Step Length";
    const char *const LABEL_NOTES = "Note Source";
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

    const char *const MSG_SCALE = "Hold down a key to use as the root, and each voice will be a scale degree in that key.";
    const char *const MSG_RECORD = "Play an arpeggiation to record it. Press the space bar to enter a rest.";

    constexpr double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
    constexpr double START_DELAY_ALLOWANCE = 0.05; // if we start playing before this many ppqs after the beat, round down to the beat. Reaper seems to start a little late.
}
