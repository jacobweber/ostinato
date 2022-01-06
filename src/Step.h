#pragma once

#include "Constants.h"

constexpr voicenum_t MAX_ACTUAL_VOICES =
        2 +
        static_cast<voicenum_t>((constants::MAX_STEPS - 1) * (constants::MAX_NOTES - 1) / (constants::MAX_VOICES - 1));

struct CurrentStep {
    std::array<bool, MAX_ACTUAL_VOICES> voices{};
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
    std::array<UpdatedStep, constants::MAX_STEPS> steps;
    stepnum_t numSteps;
    voicenum_t numVoices;
};
