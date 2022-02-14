#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include <vector>

#include "StepStrip.h"
#include "../State.h"
#include "../Constants.h"

class StepStrips : public juce::Component {
public:
    explicit StepStrips(State &_state);

    void mouseDown(const juce::MouseEvent& event) override;

    void mouseMove(const juce::MouseEvent& event) override;

    void mouseExit(const juce::MouseEvent&) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseUp(const juce::MouseEvent& event) override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void refreshActiveStep();

    void refresh();

    void refreshSteps();

private:
    bool findMouseStepVoice(juce::MouseEvent event);

private:
    State &state;
    bool oldPlaying{false};
    int oldStepNum{0};

    int curMouseStep = -1;
    int curMouseVoice = -1;
    bool clickTurnedVoiceOn = false;
    bool isDraggingVoices = false;

    // vectors move items into their own storage, but components can't be copied/moved
    // but unique_ptr can be moved, so the vector assumes ownership
    std::vector<std::unique_ptr<StepStrip>> strips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrips)
};
