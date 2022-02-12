#include <juce_audio_utils/juce_audio_utils.h>

#include "StateHelper.h"
#include "State.h"
#include "Stretcher.h"

void StateHelper::setGrid(State &state, juce::String &grid) {
    int numSteps = grid.indexOf("\n");
    int numVoices = grid.length() / (numSteps + 1);
    *(state.stepsParameter) = numSteps - 1; // index
    *(state.voicesParameter) = numVoices - 1; // index
    for (size_t i = 0; i < static_cast<size_t>(numSteps); i++) {
        for (size_t j = 0; j < static_cast<size_t>(numVoices); j++) {
            *(state.stepState[i].voiceParameters[j]) = grid[(numVoices - 1 - static_cast<int>(j)) * (numSteps + 1) + static_cast<int>(i)] == '*';
        }
        *(state.stepState[i].powerParameter) = true;
    }
}

juce::String StateHelper::getGrid(State &state) {
    int numSteps = state.stepsParameter->getIndex() + 1; // index
    int numVoices = state.voicesParameter->getIndex() + 1; // index
    juce::String grid = juce::String();
    for (int j = numVoices - 1; j >= 0; j--) {
        for (size_t i = 0; i < static_cast<size_t>(numSteps); i++) {
            grid += state.stepState[i].voiceParameters[static_cast<size_t>(j)]->get() ? '*' : '-';
        }
        grid += '\n';
    }
    return grid;
}

juce::String StateHelper::getGrid(Stretcher::StretchedResult &state) {
    juce::String grid = juce::String();
    for (int j = state.numVoices - 1; j >= 0; j--) {
        for (size_t i = 0; i < state.steps.size(); i++) {
            grid += state.steps[i].voices[static_cast<size_t>(j)] ? '*' : '-';
        }
        grid += '\n';
    }
    return grid;
}
