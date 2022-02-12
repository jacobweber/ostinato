#pragma once

#include <vector>

class Voicings {
public:
    const std::vector<int> triad = {0, 2, 4};
    const std::vector<int> seventh = {0, 2, 4, 6};
    const std::vector<int> ninth = {0, 2, 4, 6, 8};
    const std::vector<int> eleventh = {0, 2, 4, 6, 8, 10};
    const std::vector<int> sus4 = {0, 3, 4};
    const std::vector<int> sus2 = {0, 1, 4};
    const std::vector<int> mu = {0, 1, 2, 4};
    const std::vector<int> sixth = {0, 2, 4, 5};
    const std::vector<int> power = {0, 4};
    const std::vector<int> quartal = {0, 3, 6};

    const std::vector<std::vector<int>> allVoicings = {triad, seventh, ninth, eleventh, sus4, sus2, mu, sixth, power, quartal};
};
