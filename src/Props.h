#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

namespace props {
    const size_t MAX_STEPS = 16;
    const size_t MAX_VOICES = 8;
    const size_t MAX_OCTAVES = 4;

    const juce::Colour COLOR_STANDARD = juce::Colours::white;
    const juce::Colour COLOR_OUTLINE = juce::Colours::grey;
    const juce::Colour COLOR_HIGHLIGHT = juce::Colours::red;

    const juce::String LABEL_TIE = "Tie";
    const juce::String LABEL_STEPS = "Steps";
    const juce::String LABEL_VOICES = "Steps";
    const juce::String LABEL_RATE = "Step Length";

    const juce::String TOOLTIP_RANDOM = "Randomize settings";
    const juce::String TOOLTIP_VOICES_CLEAR = "Turn off all voices for this step";
    const juce::String TOOLTIP_VOICES_FILL = "Turn on all voices for this step";
    const juce::String TOOLTIP_TIE = "Hold this step instead of playing the next step";
    const juce::String TOOLTIP_POWER_OFF = "Disable this step";
    const juce::String TOOLTIP_POWER_ON = "Enable this step";

    const double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
}
