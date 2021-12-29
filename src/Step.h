#pragma once

#include "Props.h"

constexpr size_t MAX_ACTUAL_VOICES =
        2 + static_cast<size_t>((props::MAX_STEPS - 1) * (props::MAX_NOTES - 1) / (props::MAX_VOICES - 1));

struct Step {
    std::array<bool, MAX_ACTUAL_VOICES> voices{};
    double length = 0.0;
    double volume = 0.0;
    int octave = 0; // index
    bool power = true;
    bool tie = false;
};
