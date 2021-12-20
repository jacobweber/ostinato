#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "../State.h"
#include "../Constants.h"

class StepStrip : public juce::Component {
public:
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    StepStrip(State &s, size_t n) : stepNum(n), state(s) {
        DBG("created strip " << stepNum);

        lengthSlider.setSliderStyle(juce::Slider::LinearBar);
        lengthSlider.setRange(0.0, 1.0);
        lengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        lengthSlider.setPopupDisplayEnabled(true, false, this);
        addAndMakeVisible(lengthSlider);
        lengthAttachment.reset(
                new SliderAttachment(state.parameters, "step" + std::to_string(stepNum) + "_length", lengthSlider));

        refresh();
    }

    ~StepStrip() override {
        DBG("destroyed strip " << stepNum);
    }

    void paint(juce::Graphics &g) override {
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText("#" + std::to_string(stepNum + 1), getLocalBounds().withHeight(20), juce::Justification::centred,
                   true);

        g.setColour(juce::Colours::red);
        g.drawRect(getLocalBounds().reduced(1), 2.0f);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(4);
        area.removeFromTop(20);
        for (size_t i = 0; i < voices.size(); i++)
            if (voices.size() > i) {
                voices[i]->setBounds(area.removeFromTop(20));
                area.removeFromTop(2);
            }
        area.removeFromTop(20);
        lengthSlider.setBounds(area.removeFromTop(20));
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
    size_t stepNum = 0;
    State &state;

    juce::Slider lengthSlider;
    std::vector<std::unique_ptr<juce::TextButton>> voices;

    std::unique_ptr<SliderAttachment> lengthAttachment;
    std::vector<std::unique_ptr<ButtonAttachment>> voicesAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrip)
};
