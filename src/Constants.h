#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

namespace constants {
    constexpr size_t MAX_NOTES = 24;
    constexpr size_t MAX_STEPS = 16;
    constexpr size_t MAX_VOICES = 8;
    constexpr size_t MAX_OCTAVES = 4;

    inline const juce::Colour COLOR_STANDARD = juce::Colours::white;
    inline const juce::Colour COLOR_SLIDER = juce::Colours::white;
    inline const juce::Colour COLOR_HIGHLIGHT = juce::Colours::white;
    inline const juce::Colour COLOR_BACKGROUND = juce::Colour(42, 42, 42);
    inline const juce::Colour COLOR_HEADER = juce::Colour(127, 127, 127);
    inline const juce::Colour COLOR_STRIP_ACTIVE = juce::Colour(55, 55, 55);
    inline const juce::Colour COLOR_STRIP_INACTIVE = juce::Colours::black;
    inline const juce::Colour COLOR_TOGGLE_ACTIVE = COLOR_HIGHLIGHT;
    inline const juce::Colour COLOR_TOGGLE_INACTIVE = juce::Colour(76, 76, 76);
    inline const juce::Colour COLOR_TOGGLE_ACTIVE_TEXT = COLOR_TOGGLE_INACTIVE;
    inline const juce::Colour COLOR_TOGGLE_INACTIVE_TEXT = COLOR_TOGGLE_ACTIVE;
    inline const juce::Colour COLOR_LIGHT_ACTIVE = COLOR_HIGHLIGHT;
    inline const juce::Colour COLOR_LIGHT_INACTIVE = COLOR_HIGHLIGHT;
    inline const juce::Colour COLOR_COMBO_BG = COLOR_TOGGLE_INACTIVE;
    inline const juce::Colour COLOR_MESSAGE_TEXT = juce::Colours::white;

    inline const char *LABEL_TIE = "Tie";
    inline const char *LABEL_STEPS = "Steps";
    inline const char *LABEL_VOICES = "Voices";
    inline const char *LABEL_RATE = "Step Length";
    inline const char *LABEL_NOTES = "Note Source";
    inline const char *LABEL_LENGTH = "Length";
    inline const char *LABEL_OCTAVE = "Octave";

    inline const char *TOOLTIP_RECORD = "Record pattern";
    inline const char *TOOLTIP_STRETCH = "Stretch pattern to fit pressed keys";
    inline const char *TOOLTIP_RANDOM = "Randomize settings";
    inline const char *TOOLTIP_VOICES_CLEAR = "Turn off all voices for this step";
    inline const char *TOOLTIP_VOICES_FILL = "Turn on all voices for this step";
    inline const char *TOOLTIP_TIE = "Hold this step instead of playing the next step";
    inline const char *TOOLTIP_POWER_OFF = "Disable this step";
    inline const char *TOOLTIP_POWER_ON = "Enable this step";

    inline const char *MSG_SCALE = "Hold down a key to use as the root, and each voice will be a scale degree in that key.";
    inline const char *MSG_RECORD = "Play an arpeggiation to record it. Press the space bar or sustain pedal to enter a rest.";
    inline const char *MSG_STRETCH = "The shape of the pattern below will be stretched/shrunk to cover most pressed keys.";

    constexpr double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
    constexpr double START_DELAY_ALLOWANCE = 0.05; // if we start playing before this many ppqs after the beat, round down to the beat. Reaper seems to start a little late.
}
