#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "../State.h"

class ActiveLight : public juce::Component {
public:
    ActiveLight(State &s, int _stepNum);

    void enablementChanged() override;

    void paint(juce::Graphics &g) override;

private:
    int stepNum{0};
    State &state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActiveLight)
};
