#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "../State.h"

class StepStrip : public juce::Component {
public:
    const static size_t NUM_VOICES = 4;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    StepStrip(State &s, size_t n) : stepNum(n), state(s) {
        for (size_t i = 0; i < NUM_VOICES; i++) {
            voices.push_back(std::unique_ptr<juce::TextButton>(new juce::TextButton()));
            voices[i]->setClickingTogglesState(true);
            voices[i]->setButtonText(std::to_string(i + 1));
            voices[i]->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
            addAndMakeVisible(*voices[i]);
            voicesAttachments.push_back(std::unique_ptr<ButtonAttachment>(
                    new ButtonAttachment(state.parameters, "voice_" + std::to_string(stepNum) + "_" + std::to_string(i),
                                         *voices[i])));
        }
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
        for (size_t i = 0; i < NUM_VOICES; i++)
            if (voices.size() > i) {
                voices[i]->setBounds(area.removeFromTop(20));
                area.removeFromTop(2);
            }
    }

private:
    size_t stepNum = 0;
    State &state;

    std::vector<std::unique_ptr<juce::TextButton>> voices;

    std::vector<std::unique_ptr<ButtonAttachment>> voicesAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrip)
};
