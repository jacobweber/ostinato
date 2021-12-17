#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include <vector>

#include "../PluginProcessor.h"
#include "StepStrip.h"
#include "../State.h"

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    PluginEditor(PluginProcessor &, State &);

    ~PluginEditor() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void logMessage(const juce::String &);

    void timerCallback() override;

    PluginProcessor &getProcessor() const;

private:
    State &state;

    juce::Label timecodeDisplayLabel;
    juce::Slider speedSlider;
    std::vector<std::unique_ptr<StepStrip>> strips;
    juce::TextEditor messagesBox;

    // use unique_ptr so it can be destroyed when UI is
    // declare after components so it's destroyed before them
    std::unique_ptr<SliderAttachment> speedAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
