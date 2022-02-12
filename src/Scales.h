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
    const std::vector<int> majPent = {0, 2, 4, 7, 9};
    const std::vector<int> minPent = {0, 3, 5, 7, 10};
    const std::vector<int> blues = {0, 3, 5, 6, 7, 10};
    const std::vector<int> whole = {0, 2, 4, 6, 8, 10};
    const std::vector<int> wholeHalf = {0, 2, 3, 5, 6, 8, 9, 11};
    const std::vector<int> fourth = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};
    const std::vector<int> fifth = {0, 7, 14, 21, 28, 35, 42, 49, 56, 63, 70, 77};

    const std::vector<std::vector<int>> allScales = {major, minor_natural, minor_harmonic, locrian, dorian, phrygian,
                                                     lydian, mixolydian, majPent, minPent, blues, whole, wholeHalf,
                                                     fourth, fifth};

    // only 7-note scales allowed here
    const std::vector<std::vector<int>> chordScales = {major, minor_natural, minor_harmonic, locrian, dorian, phrygian,
                                                     lydian, mixolydian};
};
