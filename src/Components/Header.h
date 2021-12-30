#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "../PluginProcessor.h"
#include "../State.h"
#include "../Props.h"

class Header : public juce::Component {
public:
    const int ICON_SIZE = 24;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    Header(State &s, PluginProcessor &p);

    void paint(juce::Graphics &) override;

    void resized() override;

public:
    std::function<void()> onUpdateSteps = [] {};
    std::function<void()> onUpdateVoices = [] {};
    std::function<void()> onClickRandom = [] {};

private:
    State &state;
    PluginProcessor &pluginProcessor;

    juce::Font textFont{12.0f};

    juce::ImageButton stretchButton{};
    juce::ImageButton randomButton{};
    juce::Label stepsLabel{{}, props::LABEL_STEPS};
    juce::ComboBox stepsMenu;
    juce::Label voicesLabel{{}, props::LABEL_VOICES};
    juce::ComboBox voicesMenu;
    juce::Label rateLabel{{}, props::LABEL_RATE};
    juce::ComboBox rateMenu;
    juce::ComboBox rateTypeMenu;

    // use unique_ptr so it can be destroyed when UI is
    // declare after components so it's destroyed before them
    std::unique_ptr<ButtonAttachment> stretchAttachment;
    std::unique_ptr<ComboBoxAttachment> stepsAttachment;
    std::unique_ptr<ComboBoxAttachment> voicesAttachment;
    std::unique_ptr<ComboBoxAttachment> rateAttachment;
    std::unique_ptr<ComboBoxAttachment> rateTypeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Header)
};
