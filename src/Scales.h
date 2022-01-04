#pragma once

#include <vector>

class Scales {
public:
    const std::vector<int> major = {0, 2, 4, 5, 7, 9, 11};
    const std::vector<int> minor_natural = {0, 2, 3, 5, 7, 8, 10};
    const std::vector<int> minor_harmonic = {0, 2, 3, 5, 7, 8, 11};
    const std::vector<int> locrian = {0, 1, 3, 5, 6, 8, 10};
    const std::vector<int> dorian = {0, 2, 3, 5, 7, 9, 10};
    const std::vector<int> phrygian = {0, 1, 3, 5, 7, 8, 10};
    const std::vector<int> lydian = {0, 2, 4, 6, 7, 9, 11};
    const std::vector<int> mixolydian = {0, 2, 4, 5, 7, 9, 10};

    const std::vector<std::vector<int>> allScales = {major, minor_natural, minor_harmonic, locrian, dorian, phrygian,
                                                     lydian, mixolydian};
};
