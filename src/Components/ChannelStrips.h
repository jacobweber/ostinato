#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "StepStrip.h"
#include "../State.h"
#include "../Constants.h"

class ChannelStrips : public juce::Component {
public:
    ChannelStrips(State &s) : state(s) {
        for (size_t i = 0; i < MAX_STEPS; i++) {
            strips.push_back(std::unique_ptr<StepStrip>(new StepStrip(state, i)));
            addAndMakeVisible(*strips[i]);
        }
    }

    void paint(juce::Graphics &g) override {
    }

    void resized() override {
        auto area = getLocalBounds();
        juce::FlexBox channelStrips;
        for (size_t i = 0; i < MAX_STEPS; i++)
            if (strips.size() > i)
                channelStrips.items.add(juce::FlexItem(*strips[i]).withHeight((float) area.getHeight()).withWidth(100));
        channelStrips.performLayout(area.toFloat());
    }

private:
    State &state;
    std::vector<std::unique_ptr<StepStrip>> strips;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrips)
};
