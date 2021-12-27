#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "../PluginProcessor.h"
#include "../State.h"
#include "../Props.h"
#include "StepStrips.h"

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    PluginEditor(PluginProcessor &, State &);

    ~PluginEditor() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void timerCallback() override;

    PluginProcessor &getProcessor() const;

    void refreshSize();

private:
    void randomizeParams(bool stepsAndVoices);

private:
    State &state;

    juce::TooltipWindow tooltipWindow{nullptr, 500};

    juce::Font textFont{12.0f};

    juce::ImageButton stretchButton{};
    juce::ImageButton randomButton{};
    juce::Label timecodeDisplayLabel;
    juce::Label stepsLabel{{}, props::LABEL_STEPS};
    juce::ComboBox stepsMenu;
    juce::Label voicesLabel{{}, props::LABEL_VOICES};
    juce::ComboBox voicesMenu;
    juce::Label rateLabel{{}, props::LABEL_RATE};
    juce::ComboBox rateMenu;
    juce::ComboBox rateTypeMenu;
    StepStrips stepStrips{state};

    // use unique_ptr so it can be destroyed when UI is
    // declare after components so it's destroyed before them
    std::unique_ptr<ButtonAttachment> stretchAttachment;
    std::unique_ptr<ComboBoxAttachment> stepsAttachment;
    std::unique_ptr<ComboBoxAttachment> voicesAttachment;
    std::unique_ptr<ComboBoxAttachment> rateAttachment;
    std::unique_ptr<ComboBoxAttachment> rateTypeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
