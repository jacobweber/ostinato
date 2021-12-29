#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include <vector>

#include "../State.h"
#include "../Props.h"
#include "ActiveLight.h"
#include "FontAwesome.h"

constexpr int ICON_SIZE = 14;

class StepStrip : public juce::Component {
public:
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    StepStrip(State &s, size_t n) : stepNum(n), state(s) {
        DBG("created strip " << stepNum);

        addAndMakeVisible(activeLight);


        juce::Image square = FontAwesome::getInstance()->getIcon(false,
                                                                 juce::String::fromUTF8(
                                                                         reinterpret_cast<const char *>(u8"\uf0c8")),
                                                                 ICON_SIZE, props::COLOR_TOGGLE_ACTIVE,
                                                                 1);
        clearButton.setImages(true, false, true, square, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                              props::COLOR_TOGGLE_INACTIVE);
        clearButton.onClick = [this] {
            for (size_t i = 0; i < voices.size(); i++)
                *(state.stepState[stepNum].voiceParameters[i]) = false;
        };
        clearButton.setTooltip(props::TOOLTIP_VOICES_CLEAR);
        addAndMakeVisible(clearButton);

        juce::Image checkSquare = FontAwesome::getInstance()->getIcon(false,
                                                                      juce::String::fromUTF8(
                                                                              reinterpret_cast<const char *>(u8"\uf14a")),
                                                                      ICON_SIZE, props::COLOR_TOGGLE_ACTIVE,
                                                                      1);
        fillButton.setImages(true, false, true, checkSquare, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                             props::COLOR_TOGGLE_INACTIVE);
        fillButton.onClick = [this] {
            for (size_t i = 0; i < voices.size(); i++)
                *(state.stepState[stepNum].voiceParameters[i]) = true;
        };
        fillButton.setTooltip(props::TOOLTIP_VOICES_FILL);
        addAndMakeVisible(fillButton);

        addAndMakeVisible(octaveLabel);
        octaveLabel.setFont(textFont);
        octaveLabel.attachToComponent(&octaveMenu, false);
        int itemId = 1;
        for (int i = -static_cast<int>(props::MAX_OCTAVES); i <= static_cast<int>(props::MAX_OCTAVES); i++)
            octaveMenu.addItem(std::to_string(i), itemId++);
        addAndMakeVisible(octaveMenu);
        octaveAttachment = std::make_unique<ComboBoxAttachment>(
                state.parameters, "step" + std::to_string(stepNum) + "_octave", octaveMenu);

        lengthSlider.setSliderStyle(juce::Slider::LinearBar);
        lengthSlider.setRange(0.0, 1.0);
        lengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        lengthSlider.setPopupDisplayEnabled(true, false, this);
        lengthSlider.setColour(juce::Slider::ColourIds::trackColourId, props::COLOR_SLIDER);
        addAndMakeVisible(lengthSlider);
        lengthAttachment = std::make_unique<SliderAttachment>(
                state.parameters, "step" + std::to_string(stepNum) + "_length", lengthSlider);

        tieButton.setClickingTogglesState(true);
        tieButton.setButtonText(props::LABEL_TIE);
        tieButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, props::COLOR_TOGGLE_ACTIVE);
        tieButton.setColour(juce::TextButton::ColourIds::buttonColourId, props::COLOR_TOGGLE_INACTIVE);
        tieButton.setTooltip(props::TOOLTIP_TIE);
        addAndMakeVisible(tieButton);
        tieAttachment = std::make_unique<ButtonAttachment>(
                state.parameters, "step" + std::to_string(stepNum) + "_tie", tieButton);

        volSlider.setSliderStyle(juce::Slider::LinearBarVertical);
        volSlider.setRange(0.0, 1.0);
        volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        volSlider.setPopupDisplayEnabled(true, false, this);
        volSlider.setColour(juce::Slider::ColourIds::trackColourId, props::COLOR_SLIDER);
        addAndMakeVisible(volSlider);

        volAttachment = std::make_unique<SliderAttachment>(
                state.parameters, "step" + std::to_string(stepNum) + "_volume", volSlider);

        juce::Image powerOff = FontAwesome::getInstance()->getIcon(true,
                                                                   juce::String::fromUTF8(
                                                                           reinterpret_cast<const char *>(u8"\uf011")),
                                                                   ICON_SIZE, props::COLOR_TOGGLE_INACTIVE,
                                                                   1);
        powerButton.setImages(false, false, true, powerOff, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                              props::COLOR_TOGGLE_ACTIVE);
        powerButton.setClickingTogglesState(true);
        powerButton.onClick = [this] {
            powerButton.setTooltip(powerButton.getToggleState() ? props::TOOLTIP_POWER_OFF : props::TOOLTIP_POWER_ON);
            repaint();
        };
        addAndMakeVisible(powerButton);
        powerAttachment = std::make_unique<ButtonAttachment>(
                state.parameters, "step" + std::to_string(stepNum) + "_power", powerButton);
        powerButton.setTooltip(powerButton.getToggleState() ? props::TOOLTIP_POWER_OFF : props::TOOLTIP_POWER_ON);

        refresh();
    }

    ~StepStrip() override {
        DBG("destroyed strip " << stepNum);
    }

    void paint(juce::Graphics &g) override {
        auto rect = getLocalBounds().reduced(2);
        bool active = state.stepState[stepNum].powerParameter->get();
        g.setColour(active ? props::COLOR_STRIP_ACTIVE : props::COLOR_STRIP_INACTIVE);
        g.fillRect(rect);
    }

    void resized() override {
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

        for (size_t i = 0; i < voices.size(); i++)
            if (voices.size() > i) {
                voices[i]->setBounds(area.removeFromTop(20).reduced(2));
                area.removeFromTop(2);
            }

        powerButton.setBounds(area.removeFromBottom(20));

        area.removeFromTop(30);
        octaveMenu.setBounds(area.removeFromTop(24));

        area.removeFromTop(30);
        lengthSlider.setBounds(area.removeFromTop(20));

        area.removeFromTop(30);
        tieButton.setBounds(area.removeFromTop(20));

        area.removeFromTop(30);
        volSlider.setBounds(area);
    }

    void refreshActiveLight() {
        activeLight.repaint();
    }

    void refresh() {
        refreshVoices();
    }

    void refreshVoices() {
        size_t oldNumVoices = voices.size();
        size_t newNumVoices = static_cast<size_t>(state.voicesParameter->getIndex()) + 1;
        if (newNumVoices > oldNumVoices) {
            for (size_t i = oldNumVoices; i < newNumVoices; i++) {
                voices.push_back(std::make_unique<juce::TextButton>());
                voices[i]->setClickingTogglesState(true);
                voices[i]->setColour(juce::TextButton::ColourIds::buttonOnColourId, props::COLOR_TOGGLE_ACTIVE);
                voices[i]->setColour(juce::TextButton::ColourIds::buttonColourId, props::COLOR_TOGGLE_INACTIVE);
                addAndMakeVisible(*voices[i]);
                voicesAttachments.push_back(std::make_unique<ButtonAttachment>(
                        state.parameters,
                        "step" + std::to_string(stepNum) + "_voice" + std::to_string(i),
                        *voices[i]));
            }
            oldNumVoices = newNumVoices;
            resized();
        } else if (newNumVoices < oldNumVoices) {
            for (size_t i = oldNumVoices - 1; i >= newNumVoices; i--) {
                removeChildComponent(voices[i].get());
                voicesAttachments.pop_back();
                voices.pop_back();
            }
            oldNumVoices = newNumVoices;
            resized();
        }
    }

private:
    size_t stepNum{0};
    State &state;

    juce::Font textFont{12.0f};

    ActiveLight activeLight{state, stepNum};
    juce::ImageButton clearButton{};
    juce::ImageButton fillButton{};
    juce::Label octaveLabel{{}, "Octave"};
    juce::ComboBox octaveMenu;
    juce::Slider lengthSlider;
    juce::TextButton tieButton{};
    std::vector<std::unique_ptr<juce::TextButton>> voices;
    juce::Slider volSlider;
    juce::ImageButton powerButton{};

    std::vector<std::unique_ptr<ButtonAttachment>> voicesAttachments;
    std::unique_ptr<ComboBoxAttachment> octaveAttachment;
    std::unique_ptr<SliderAttachment> lengthAttachment;
    std::unique_ptr<ButtonAttachment> tieAttachment;
    std::unique_ptr<SliderAttachment> volAttachment;
    std::unique_ptr<ButtonAttachment> powerAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrip)
};
