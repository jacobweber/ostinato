#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

const size_t MAX_STEPS = 16;
const size_t MAX_VOICES = 8;
const size_t MAX_OCTAVES = 4;

const juce::Colour COLOR_STANDARD = juce::Colours::white;
const juce::Colour COLOR_OUTLINE = juce::Colours::grey;
const juce::Colour COLOR_HIGHLIGHT = juce::Colours::red;

const double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once
