#pragma once

#include <vector>

class Chords {
public:
    const std::vector<int> major = {0, 4, 7};
    const std::vector<int> major2 = {0, 2, 4, 7};
    const std::vector<int> major6 = {0, 4, 7, 9};
    const std::vector<int> dom7 = {0, 4, 7, 10};
    const std::vector<int> dom9 = {0, 4, 7, 10, 14};
    const std::vector<int> major7 = {0, 4, 7, 11};
    const std::vector<int> major9 = {0, 4, 7, 11, 14};
    const std::vector<int> minor = {0, 3, 7};
    const std::vector<int> minor2 = {0, 2, 3, 7};
    const std::vector<int> minor6 = {0, 3, 7, 9};
    const std::vector<int> minor7 = {0, 3, 7, 10};
    const std::vector<int> minor9 = {0, 3, 7, 10, 14};
    const std::vector<int> dimin = {0, 3, 6};
    const std::vector<int> halfDimin7 = {0, 3, 6, 10};
    const std::vector<int> dimin7 = {0, 3, 6, 9};

    const std::vector<std::vector<int>> allChords = {major, major2, major6, dom7, dom9, major7, major9,
        minor, minor2, minor6, minor7, minor9, dimin, halfDimin7, dimin7};
};
