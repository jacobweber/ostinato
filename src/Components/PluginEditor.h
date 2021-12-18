#pragma once

#include "juce_audio_utils/juce_audio_utils.h"

#include "../PluginProcessor.h"
#include "../State.h"
#include "ChannelStrips.h"

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    PluginEditor(PluginProcessor &, State &);

    ~PluginEditor() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void logMessage(const juce::String &);

    void timerCallback() override;

    PluginProcessor &getProcessor() const;

    void refreshSize();

private:
    State &state;

    juce::Label timecodeDisplayLabel;

    juce::Font textFont{12.0f};

    juce::Label stepsLabel{{}, "Steps"};
    juce::ComboBox stepsMenu;
    juce::Label voicesLabel{{}, "Voices"};
    juce::ComboBox voicesMenu;
    juce::Label rateLabel{{}, "Step length"};
    juce::ComboBox rateMenu;
    ChannelStrips channelStrips{state};
    juce::TextEditor messagesBox;

    // use unique_ptr so it can be destroyed when UI is
    // declare after components so it's destroyed before them
    std::unique_ptr<ComboBoxAttachment> stepsAttachment;
    std::unique_ptr<ComboBoxAttachment> voicesAttachment;
    std::unique_ptr<ComboBoxAttachment> rateAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
