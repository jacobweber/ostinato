#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "../State.h"
#include "../Props.h"

class ActiveLight : public juce::Component {
public:
    ActiveLight(State &s, size_t n) : stepNum(n), state(s) {
    }

    void enablementChanged() override {
        repaint();
    }

    void paint(juce::Graphics &g) override {
        bool highlighted = state.playing && state.stepIndex == stepNum;
        g.setColour(highlighted ? props::COLOR_LIGHT_ACTIVE : (isEnabled() ? props::COLOR_LIGHT_INACTIVE
                                                                           : props::COLOR_LIGHT_INACTIVE.withAlpha(
                        0.3f)));
        juce::Rectangle<float> outer = getLocalBounds().toFloat();
        g.drawEllipse(outer.reduced(1), 1.5f);
        if (highlighted)
            g.fillEllipse(outer.reduced(3));
    }

private:
    size_t stepNum{0};
    State &state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActiveLight)
};
