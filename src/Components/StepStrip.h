#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "../State.h"
#include "../Constants.h"
#include "ActiveLight.h"
#include "IconButtonLookAndFeel.h"

constexpr int ICON_SIZE = 14;

class StepStrip : public juce::Component {
public:
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    StepStrip(State &s, size_t n) : stepNum(n), state(s) {
        DBG("created strip " << stepNum);

        addAndMakeVisible(activeLight);
        clearButton.setLookAndFeel(&clearButtonLAF);
        clearButton.onClick = [this] {
            for (size_t i = 0; i < voices.size(); i++)
                *(state.stepState[stepNum].voiceParameters[i]) = false;
        };
        addAndMakeVisible(clearButton);
        fillButton.setLookAndFeel(&fillButtonLAF);
        fillButton.onClick = [this] {
            for (size_t i = 0; i < voices.size(); i++)
                *(state.stepState[stepNum].voiceParameters[i]) = true;
        };
        addAndMakeVisible(fillButton);

        lengthSlider.setSliderStyle(juce::Slider::LinearBar);
        lengthSlider.setRange(0.0, 1.0);
        lengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        lengthSlider.setPopupDisplayEnabled(true, false, this);
        lengthSlider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::grey);
        addAndMakeVisible(lengthSlider);
        lengthAttachment.reset(
                new SliderAttachment(state.parameters, "step" + std::to_string(stepNum) + "_length", lengthSlider));

        refresh();
    }

    ~StepStrip() override {
        DBG("destroyed strip " << stepNum);
    }

    void paint(juce::Graphics &g) override {
        g.setColour(juce::Colours::grey);
        g.drawRect(getLocalBounds().reduced(1), 2.0f);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(4);

        auto iconArea = area.removeFromTop(30);
        juce::FlexBox stepsBox;
        stepsBox.alignItems = juce::FlexBox::AlignItems::center;
        stepsBox.justifyContent = juce::FlexBox::JustifyContent::center;
        stepsBox.items.add(juce::FlexItem(activeLight).withMargin(3).withHeight(ICON_SIZE).withWidth(ICON_SIZE));
        stepsBox.items.add(juce::FlexItem(clearButton).withMargin(3).withHeight(ICON_SIZE).withWidth(ICON_SIZE));
        stepsBox.items.add(juce::FlexItem(fillButton).withMargin(3).withHeight(ICON_SIZE).withWidth(ICON_SIZE));
        stepsBox.performLayout(iconArea.toFloat());

        for (size_t i = 0; i < voices.size(); i++)
            if (voices.size() > i) {
                voices[i]->setBounds(area.removeFromTop(20));
                area.removeFromTop(2);
            }
        area.removeFromTop(20);
        lengthSlider.setBounds(area.removeFromTop(20));
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
                voices[i]->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
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

    ActiveLight activeLight{state, stepNum};
    IconButtonLookAndFeel clearButtonLAF{juce::String::fromUTF8(u8"\uf0c8"), ICON_SIZE}; // square
    IconButtonLookAndFeel fillButtonLAF{juce::String::fromUTF8(u8"\uf14a"), ICON_SIZE}; // check-square
    juce::TextButton clearButton;
    juce::TextButton fillButton;
    juce::Slider lengthSlider;
    std::vector<std::unique_ptr<juce::TextButton>> voices;

    std::unique_ptr<SliderAttachment> lengthAttachment;
    std::vector<std::unique_ptr<ButtonAttachment>> voicesAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrip)
};
