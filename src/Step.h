#pragma once

#include "Constants.h"

constexpr int MAX_ACTUAL_VOICES =
        2 + ((constants::MAX_STEPS - 1) * (constants::MAX_NOTES - 1) / (constants::MAX_VOICES - 1));

struct CurrentStep {
    std::array<bool, MAX_ACTUAL_VOICES> voices{};
    int numVoices = 1;
    double length = 0.0;
    double volume = 0.0;
    int octave = 0; // index
    bool power = true;
    bool tie = false;
};

struct UpdatedStep {
    std::array<bool, constants::MAX_VOICES> voices;
    float length;
    float volume;
};
struct UpdatedSteps {
    std::array<UpdatedStep, static_cast<size_t>(constants::MAX_STEPS)> steps;
    int numSteps;
    int numVoices;
};
