#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "StepStrip.h"
#include "../State.h"
#include "../Constants.h"
#include "ActiveLight.h"
#include "FontAwesome.h"

StepStrip::StepStrip(State &_state, int _stepNum) : stepNum(_stepNum), state(_state) {
    DBG("created strip " << stepNum);
    setInterceptsMouseClicks(false, true);

    addAndMakeVisible(activeLight);

    juce::Image square = FontAwesome::getInstance()->getIcon(false,
                                                                juce::String::fromUTF8(
                                                                        reinterpret_cast<const char *>(u8"\uf0c8")),
                                                                ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                                1);
    clearButton.setImages(true, false, true, square, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                            constants::COLOR_TOGGLE_INACTIVE);
    clearButton.onClick = [this] {
        for (size_t i = 0; i < voices.size(); i++)
            *(state.stepState[static_cast<size_t>(stepNum)].voiceParameters[i]) = false;
    };
    clearButton.setTooltip(constants::TOOLTIP_VOICES_CLEAR);
    addAndMakeVisible(clearButton);

    juce::Image checkSquare = FontAwesome::getInstance()->getIcon(false,
                                                                    juce::String::fromUTF8(
                                                                            reinterpret_cast<const char *>(u8"\uf14a")),
                                                                    ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                                    1);
    fillButton.setImages(true, false, true, checkSquare, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                            constants::COLOR_TOGGLE_INACTIVE);
    fillButton.onClick = [this] {
        for (size_t i = 0; i < voices.size(); i++)
            *(state.stepState[static_cast<size_t>(stepNum)].voiceParameters[i]) = true;
    };
    fillButton.setTooltip(constants::TOOLTIP_VOICES_FILL);
    addAndMakeVisible(fillButton);

    if (stepNum == 0) {
        addAndMakeVisible(octaveLabel);
        octaveLabel.setFont(textFont);
        octaveLabel.attachToComponent(&octaveMenu, false);
    }
    int itemId = 1;
    for (int i = constants::MAX_OCTAVES; i >= -constants::MAX_OCTAVES; i--)
        octaveMenu.addItem(std::to_string(i), itemId++);
    addAndMakeVisible(octaveMenu);
    octaveAttachment = std::make_unique<ComboBoxAttachment>(
            state.parameters, "step" + std::to_string(stepNum) + "_octave", octaveMenu);

    if (stepNum == 0) {
        addAndMakeVisible(lengthLabel);
        lengthLabel.setFont(textFont);
        lengthLabel.attachToComponent(&lengthSlider, false);
    }
    lengthSlider.setSliderStyle(juce::Slider::LinearBar);
    lengthSlider.setRange(0.0, 1.0);
    lengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    lengthSlider.setPopupDisplayEnabled(true, false, this);
    lengthSlider.setColour(juce::Slider::ColourIds::trackColourId, constants::COLOR_SLIDER);
    addAndMakeVisible(lengthSlider);
    lengthAttachment = std::make_unique<SliderAttachment>(
            state.parameters, "step" + std::to_string(stepNum) + "_length", lengthSlider);

    tieButton.setClickingTogglesState(true);
    tieButton.setButtonText(constants::LABEL_TIE);
    tieButton.setTooltip(constants::TOOLTIP_TIE);
    addAndMakeVisible(tieButton);
    tieAttachment = std::make_unique<ButtonAttachment>(
            state.parameters, "step" + std::to_string(stepNum) + "_tie", tieButton);

    volSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    volSlider.setRange(0.0, 1.0);
    volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    volSlider.setPopupDisplayEnabled(true, false, this);
    volSlider.setColour(juce::Slider::ColourIds::trackColourId, constants::COLOR_SLIDER);
    addAndMakeVisible(volSlider);

    volAttachment = std::make_unique<SliderAttachment>(
            state.parameters, "step" + std::to_string(stepNum) + "_volume", volSlider);

    juce::Image powerOff = FontAwesome::getInstance()->getIcon(true,
                                                                juce::String::fromUTF8(
                                                                        reinterpret_cast<const char *>(u8"\uf011")),
                                                                ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                                1);
    powerButton.setImages(false, false, true, powerOff, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, {});
    powerButton.setClickingTogglesState(true);
    powerButton.onClick = [this] {
        powerButton.setTooltip(
                powerButton.getToggleState() ? constants::TOOLTIP_POWER_OFF : constants::TOOLTIP_POWER_ON);
        repaint();
    };
    addAndMakeVisible(powerButton);
    powerAttachment = std::make_unique<ButtonAttachment>(
            state.parameters, "step" + std::to_string(stepNum) + "_power", powerButton);
    powerButton.setTooltip(
            powerButton.getToggleState() ? constants::TOOLTIP_POWER_OFF : constants::TOOLTIP_POWER_ON);

    refresh();
}

StepStrip::~StepStrip() {
    DBG("destroyed strip " << stepNum);
}

void StepStrip::paint(juce::Graphics &) {
}

void StepStrip::paintOverChildren(juce::Graphics &g) {
    bool active = state.stepState[static_cast<size_t>(stepNum)].powerParameter->get();
    if (!active) {
        auto rect = getLocalBounds();
        g.setColour(juce::Colour(0.0f, 0.0f, 0.0f, 0.5f));
        g.fillRect(rect);
    }
}

void StepStrip::resized() {
    auto area = getLocalBounds().reduced(6);

    auto iconArea = area.removeFromTop(30);
    juce::FlexBox stepsBox;
    stepsBox.alignContent = juce::FlexBox::AlignContent::center;
    stepsBox.justifyContent = juce::FlexBox::JustifyContent::center;
    stepsBox.items.add(
            juce::FlexItem(activeLight).withMargin(3).withHeight(ICON_SIZE).withWidth(ICON_SIZE));
    stepsBox.items.add(juce::FlexItem(clearButton).withMargin(3).withHeight(ICON_SIZE).withWidth(ICON_SIZE));
    stepsBox.items.add(juce::FlexItem(fillButton).withMargin(3).withHeight(ICON_SIZE).withWidth(ICON_SIZE));
    stepsBox.performLayout(iconArea.toFloat());

    for (int i = static_cast<int>(voices.size()) - 1; i >= 0; i--)
        if (static_cast<int>(voices.size()) > i) {
            voices[static_cast<size_t>(i)]->setBounds(area.removeFromTop(20).reduced(2));
            area.removeFromTop(2);
        }

    powerButton.setBounds(area.removeFromBottom(20));

    area.removeFromTop(30);
    octaveMenu.setBounds(area.removeFromTop(24));

    area.removeFromTop(30);
    lengthSlider.setBounds(area.removeFromTop(20));

    area.removeFromTop(5);
    tieButton.setBounds(area.removeFromTop(20));

    area.removeFromTop(30);
    auto sliderArea = area.withSizeKeepingCentre(juce::jmax(area.getWidth() / 2, 30), area.getHeight());
    volSlider.setBounds(sliderArea);
}

void StepStrip::refreshActiveLight() {
    activeLight.repaint();
}

void StepStrip::refresh() {
    refreshVoices();
}

void StepStrip::refreshVoices() {
    int oldNumVoices = static_cast<int>(voices.size());
    int newNumVoices = state.voicesParameter->getIndex() + 1;
    if (newNumVoices > oldNumVoices) {
        for (size_t i = static_cast<size_t>(oldNumVoices); i < static_cast<size_t>(newNumVoices); i++) {
            voices.push_back(std::make_unique<juce::TextButton>());
            voices[i]->setClickingTogglesState(true);
            voices[i]->setInterceptsMouseClicks(false, false);
            addAndMakeVisible(*voices[i]);
            voicesAttachments.push_back(std::make_unique<ButtonAttachment>(
                    state.parameters,
                    "step" + std::to_string(stepNum) + "_voice" + std::to_string(i),
                    *voices[i]));
        }
        resized();
    } else if (newNumVoices < oldNumVoices) {
        for (size_t i = static_cast<size_t>(oldNumVoices) - 1; i >= static_cast<size_t>(newNumVoices); i--) {
            removeChildComponent(voices[i].get());
            voicesAttachments.pop_back();
            voices.pop_back();
        }
        resized();
    }
}

int StepStrip::getVoiceForPoint(int x, int y) {
    int size = static_cast<int>(voices.size());
    for (int i = 0; i < size; i++) {
        if (voices[static_cast<size_t>(i)]->getBounds().contains(x, y)) return i;
    }
    return -1;
}

void StepStrip::hoverVoice(int voiceNum, bool over) {
    voices[static_cast<size_t>(voiceNum)]->setState(over ? juce::Button::buttonOver : juce::Button::buttonNormal);
}
