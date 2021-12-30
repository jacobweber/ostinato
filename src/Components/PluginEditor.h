#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "../PluginProcessor.h"
#include "../State.h"
#include "../Props.h"
#include "LookAndFeel.h"
#include "StepStrips.h"
#include "Header.h"

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    PluginEditor(PluginProcessor &, State &);

    void paint(juce::Graphics &) override;

    void resized() override;

    void timerCallback() override;

    void refreshSize();

private:
    void randomizeParams(bool stepsAndVoices);

private:
    State &state;
    PluginProcessor &pluginProcessor; // store subclass reference so we don't need to cast dynamically

    LookAndFeel lookAndFeel{};

    juce::TooltipWindow tooltipWindow{nullptr, 500};

    Header header{state, pluginProcessor};
    StepStrips stepStrips{state};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
