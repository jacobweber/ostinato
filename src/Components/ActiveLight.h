#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "../State.h"
#include "../Constants.h"

class ActiveLight : public juce::Component {
public:
    ActiveLight(State &s, size_t n) : stepNum(n), state(s) {
    }

    void paint(juce::Graphics &g) override {
        g.setColour(COLOR_STANDARD);
        juce::Rectangle<float> outer = getLocalBounds().toFloat();
        g.drawEllipse(outer.reduced(1), 1.5f);
        if (state.playing && state.stepIndex == stepNum) {
            g.setColour(COLOR_HIGHLIGHT);
            g.fillEllipse(outer.reduced(3));
        }
    }

private:
    size_t stepNum{0};
    State &state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActiveLight)
};
