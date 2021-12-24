#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "../State.h"
#include "../Constants.h"
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
                                                                 ICON_SIZE, COLOR_STANDARD,
                                                                 1);
        clearButton.setImages(true, false, true, square, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, COLOR_HIGHLIGHT);
        clearButton.onClick = [this] {
            for (size_t i = 0; i < voices.size(); i++)
                *(state.stepState[stepNum].voiceParameters[i]) = false;
        };
        addAndMakeVisible(clearButton);

        juce::Image checkSquare = FontAwesome::getInstance()->getIcon(false,
                                                                      juce::String::fromUTF8(
                                                                              reinterpret_cast<const char *>(u8"\uf14a")),
                                                                      ICON_SIZE, COLOR_STANDARD,
                                                                      1);
        fillButton.setImages(true, false, true, checkSquare, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, COLOR_HIGHLIGHT);
        fillButton.onClick = [this] {
            for (size_t i = 0; i < voices.size(); i++)
                *(state.stepState[stepNum].voiceParameters[i]) = true;
        };
        addAndMakeVisible(fillButton);

        addAndMakeVisible(octaveLabel);
        octaveLabel.setFont(textFont);
        octaveLabel.attachToComponent(&octaveMenu, false);
        int itemId = 1;
        for (int i = -static_cast<int>(MAX_OCTAVES); i <= static_cast<int>(MAX_OCTAVES); i++)
            octaveMenu.addItem(std::to_string(i), itemId++);
        addAndMakeVisible(octaveMenu);
        octaveAttachment.reset(
                new ComboBoxAttachment(state.parameters, "step" + std::to_string(stepNum) + "_octave", octaveMenu));

        lengthSlider.setSliderStyle(juce::Slider::LinearBar);
        lengthSlider.setRange(0.0, 1.0);
        lengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        lengthSlider.setPopupDisplayEnabled(true, false, this);
        lengthSlider.setColour(juce::Slider::ColourIds::trackColourId, COLOR_OUTLINE);
        addAndMakeVisible(lengthSlider);
        lengthAttachment.reset(
                new SliderAttachment(state.parameters, "step" + std::to_string(stepNum) + "_length", lengthSlider));

        tieButton.setClickingTogglesState(true);
        tieButton.setButtonText("Tie");
        tieButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, COLOR_HIGHLIGHT);
        addAndMakeVisible(tieButton);
        tieAttachment.reset(
                new ButtonAttachment(state.parameters, "step" + std::to_string(stepNum) + "_tie", tieButton));

        volSlider.setSliderStyle(juce::Slider::LinearBarVertical);
        volSlider.setRange(0.0, 1.0);
        volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        volSlider.setPopupDisplayEnabled(true, false, this);
        volSlider.setColour(juce::Slider::ColourIds::trackColourId, COLOR_OUTLINE);
        addAndMakeVisible(volSlider);

        volAttachment.reset(
                new SliderAttachment(state.parameters, "step" + std::to_string(stepNum) + "_volume", volSlider));

        juce::Image powerOff = FontAwesome::getInstance()->getIcon(true,
                                                                   juce::String::fromUTF8(
                                                                           reinterpret_cast<const char *>(u8"\uf011")),
                                                                   ICON_SIZE, COLOR_STANDARD,
                                                                   1);
        powerButton.setImages(false, false, true, powerOff, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                              COLOR_HIGHLIGHT);
        powerButton.setClickingTogglesState(true);
        addAndMakeVisible(powerButton);
        powerAttachment.reset(
                new ButtonAttachment(state.parameters, "step" + std::to_string(stepNum) + "_power", powerButton));

        refresh();
    }

    ~StepStrip() override {
        DBG("destroyed strip " << stepNum);
    }

    void paint(juce::Graphics &g) override {
        g.setColour(COLOR_OUTLINE);
        g.drawRect(getLocalBounds().reduced(1), 2.0f);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(4);

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
                voices[i]->setBounds(area.removeFromTop(20));
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
        size_t newNumVoices = static_cast<size_t>(state.voicesParameter->getIndex() + 1);
        if (newNumVoices > oldNumVoices) {
            for (size_t i = oldNumVoices; i < newNumVoices; i++) {
                voices.push_back(std::unique_ptr<juce::TextButton>(new juce::TextButton()));
                voices[i]->setClickingTogglesState(true);
                voices[i]->setButtonText(std::to_string(i + 1));
                voices[i]->setColour(juce::TextButton::ColourIds::buttonOnColourId, COLOR_HIGHLIGHT);
                addAndMakeVisible(*voices[i]);
                voicesAttachments.push_back(std::unique_ptr<ButtonAttachment>(
                        new ButtonAttachment(state.parameters,
                                             "step" + std::to_string(stepNum) + "_voice" + std::to_string(i),
                                             *voices[i])));
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
