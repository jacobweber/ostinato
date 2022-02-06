#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "Stretcher.h"

class StateHelper {
public:
    static void setGrid(State &state, juce::String &grid);

    static juce::String getGrid(State &state);

    static juce::String getGrid(Stretcher::StretchedResult &state);
};
