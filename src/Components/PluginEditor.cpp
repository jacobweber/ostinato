#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor &_processor, State &_state)
        : AudioProcessorEditor(&_processor), state(_state), pluginProcessor(_processor) {

    setLookAndFeel(&lookAndFeel);

    header.onUpdateSteps = [this] {
        refreshSize();
        stepStrips.refresh();
    };
    header.onUpdateVoices = [this] {
        refreshSize();
        stepStrips.refresh();
    };
    header.onUpdateMode = [this] {
        stepStrips.refresh();
    };
    header.onClickRecord = [this] {
        stepStrips.refresh();
    };
    header.onClickRandom = [this] {
        state.randomizeParams(false, false, false);
    };
    header.onClickHelp = [this] {
        showHelp();
    };
    addAndMakeVisible(header);
    addAndMakeVisible(stepStrips);

    refreshSize(); // resize after initialization
    startTimerHz(60);

    setWantsKeyboardFocus(true);
    addKeyListener(this);
}

PluginEditor::~PluginEditor() {
    removeKeyListener(this);
    setLookAndFeel(nullptr);
}

void PluginEditor::showHelp() {
    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
        "Ostinato Help",
        "Keyboard shortcuts:\n\n"
        "left/right arrows: remove/add steps\n"
        "down/up arrows: remove/add voices\n"
        "shift + left/right arrows: shift steps left/right\n"
        "shift + down/up arrows: shift voices down/up\n",
        "OK", this);
}

bool PluginEditor::keyPressed(const juce::KeyPress &key) {
    if (state.recordButton && key.isKeyCode(juce::KeyPress::spaceKey)) {
        state.recordedRest = true;
        return true;
    }
    int keyCode = key.getKeyCode();
    bool shift = key.getModifiers().isShiftDown();
    if (shift) {
        if (keyCode == juce::KeyPress::leftKey) {
            state.shiftStepsLeft();
            return true;
        } else if (keyCode == juce::KeyPress::rightKey) {
            state.shiftStepsRight();
            return true;
        } else if (keyCode == juce::KeyPress::downKey) {
            state.shiftVoicesDown();
            return true;
        } else if (keyCode == juce::KeyPress::upKey) {
            state.shiftVoicesUp();
            return true;
        }
    } else {
        if (keyCode == juce::KeyPress::leftKey) {
            *(state.stepsParameter) = std::max(0, state.stepsParameter->getIndex() - 1);
            return true;
        } else if (keyCode == juce::KeyPress::rightKey) {
            *(state.stepsParameter) = std::min(constants::MAX_STEPS - 1, state.stepsParameter->getIndex() + 1);
            return true;
        } else if (keyCode == juce::KeyPress::downKey) {
            *(state.voicesParameter) = std::max(0, state.voicesParameter->getIndex() - 1);
            return true;
        } else if (keyCode == juce::KeyPress::upKey) {
           *(state.voicesParameter) = std::min(constants::MAX_VOICES - 1, state.voicesParameter->getIndex() + 1);
            return true;
        }
    }
    return false;
}

bool PluginEditor::keyPressed(const juce::KeyPress &key, juce::Component *) {
    return keyPressed(key);
}

void PluginEditor::paint(juce::Graphics &g) {
    juce::ColourGradient gradient(constants::COLOR_BACKGROUND, 0, 0, (constants::COLOR_BACKGROUND).brighter(.25f),
                                  static_cast<float>(getWidth()),
                                  0.0f, false);
    g.setGradientFill(gradient);
    g.fillAll();
}

void PluginEditor::resized() {
    auto area = getLocalBounds();
    header.setBounds(area.removeFromTop(133));
    stepStrips.setBounds(area);
}

void PluginEditor::timerCallback() {
    if (!grabbedFocus && isShowing()) {
        grabKeyboardFocus();
        grabbedFocus = true;
    }
    stepStrips.refreshActiveStep();
    header.timerCallback();
}

void PluginEditor::refreshSize() {
    int numSteps = state.stepsParameter->getIndex() + 1;
    int width = juce::jmin(1200, juce::jmax(800, 100 * numSteps));
    setSize(width, 648);
}

