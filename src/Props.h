#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

namespace props {
    inline const size_t MAX_STEPS = 16;
    inline const size_t MAX_VOICES = 8;
    inline const size_t MAX_OCTAVES = 4;

    inline const juce::Colour COLOR_STANDARD = juce::Colours::white;
    inline const juce::Colour COLOR_OUTLINE = juce::Colours::grey;
    inline const juce::Colour COLOR_HIGHLIGHT = juce::Colours::red;

    inline const juce::String LABEL_TIE = "Tie";
    inline const juce::String LABEL_STEPS = "Steps";
    inline const juce::String LABEL_VOICES = "Steps";
    inline const juce::String LABEL_RATE = "Step Length";

    inline const juce::String TOOLTIP_RANDOM = "Randomize settings";
    inline const juce::String TOOLTIP_VOICES_CLEAR = "Turn off all voices for this step";
    inline const juce::String TOOLTIP_VOICES_FILL = "Turn on all voices for this step";
    inline const juce::String TOOLTIP_TIE = "Hold this step instead of playing the next step";
    inline const juce::String TOOLTIP_POWER_OFF = "Disable this step";
    inline const juce::String TOOLTIP_POWER_ON = "Enable this step";

    inline const double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
}
