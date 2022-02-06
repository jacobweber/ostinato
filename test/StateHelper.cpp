#include <juce_audio_utils/juce_audio_utils.h>

#include "StateHelper.h"
#include "State.h"
#include "Stretcher.h"

void StateHelper::setGrid(State &state, juce::String &grid) {
    auto numSteps = static_cast<stepnum_t>(grid.indexOf("\n"));
    auto numVoices = static_cast<voicenum_t>(grid.length()) / (numSteps + 1);
    *(state.stepsParameter) = static_cast<int>(numSteps - 1); // index
    *(state.voicesParameter) = static_cast<int>(numVoices - 1); // index
    for (stepnum_t i = 0; i < numSteps; i++) {
        for (voicenum_t j = 0; j < numVoices; j++) {
            *(state.stepState[i].voiceParameters[j]) = grid[static_cast<int>((numVoices - 1 - j) * (numSteps + 1) + i)] == '*';
        }
        *(state.stepState[i].powerParameter) = true;
    }
}

juce::String StateHelper::getGrid(State &state) {
    auto numSteps = static_cast<stepnum_t>(state.stepsParameter->getIndex()) + 1; // index
    auto numVoices = static_cast<voicenum_t>(state.voicesParameter->getIndex()) + 1; // index
    juce::String grid = juce::String();
    for (int j = static_cast<int>(numVoices) - 1; j >= 0; j--) {
        for (stepnum_t i = 0; i < numSteps; i++) {
            grid += state.stepState[i].voiceParameters[j]->get() ? '*' : '-';
        }
        grid += '\n';
    }
    return grid;
}

juce::String StateHelper::getGrid(Stretcher::StretchedResult &state) {
    juce::String grid = juce::String();
    for (int j = static_cast<int>(state.numVoices) - 1; j >= 0; j--) {
        for (stepnum_t i = 0; i < state.steps.size(); i++) {
            grid += state.steps[i].voices[static_cast<size_t>(j)] ? '*' : '-';
        }
        grid += '\n';
    }
    return grid;
}
