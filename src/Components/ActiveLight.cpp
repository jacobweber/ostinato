#include "juce_gui_basics/juce_gui_basics.h"

#include "ActiveLight.h"
#include "../State.h"
#include "../Constants.h"

ActiveLight::ActiveLight(State &_state, int _stepNum) : stepNum(_stepNum), state(_state) {
}

void ActiveLight::enablementChanged() {
    repaint();
}

void ActiveLight::paint(juce::Graphics &g) {
    bool highlighted = state.playing && state.stepNum == stepNum;
    if (isEnabled()) {
        g.setColour(highlighted ? constants::COLOR_LIGHT_ACTIVE : constants::COLOR_LIGHT_INACTIVE);
    } else {
        g.setColour(constants::COLOR_LIGHT_INACTIVE.withAlpha(0.3f));
    }
    juce::Rectangle<float> outer = getLocalBounds().toFloat();
    g.drawEllipse(outer.reduced(1), 1.5f);
    if (highlighted)
        g.fillEllipse(outer.reduced(3));
}
