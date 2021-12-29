#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

namespace props {
    constexpr size_t MAX_NOTES = 24;
    constexpr size_t MAX_STEPS = 16;
    constexpr size_t MAX_VOICES = 8;
    constexpr size_t MAX_OCTAVES = 4;

    const juce::Colour COLOR_STANDARD = juce::Colours::white;
    const juce::Colour COLOR_OUTLINE = juce::Colours::grey;
    const juce::Colour COLOR_HIGHLIGHT = juce::Colours::red;
    const juce::Colour COLOR_BACKGROUND = juce::Colour(42, 42, 42);
    const juce::Colour COLOR_HEADER = juce::Colour(127, 127, 127);
    const juce::Colour COLOR_STRIP_ACTIVE = juce::Colour(55, 55, 55);
    const juce::Colour COLOR_STRIP_INACTIVE = juce::Colour(0, 0, 0);

    const juce::String LABEL_TIE = "Tie";
    const juce::String LABEL_STEPS = "Steps";
    const juce::String LABEL_VOICES = "Steps";
    const juce::String LABEL_RATE = "Step Length";

    const juce::String TOOLTIP_STRETCH = "Stretch pattern to fit played notes";
    const juce::String TOOLTIP_RANDOM = "Randomize settings";
    const juce::String TOOLTIP_VOICES_CLEAR = "Turn off all voices for this step";
    const juce::String TOOLTIP_VOICES_FILL = "Turn on all voices for this step";
    const juce::String TOOLTIP_TIE = "Hold this step instead of playing the next step";
    const juce::String TOOLTIP_POWER_OFF = "Disable this step";
    const juce::String TOOLTIP_POWER_ON = "Enable this step";

    constexpr double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
    constexpr double START_DELAY_ALLOWANCE = 0.05; // if we start playing before this many ppqs after the beat, round down to the beat. Reaper seems to start a little late.
}
