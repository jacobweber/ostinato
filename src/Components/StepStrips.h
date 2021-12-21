#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

#include "StepStrip.h"
#include "../State.h"
#include "../Constants.h"

class StepStrips : public juce::Component {
public:
    StepStrips(State &s) : state(s) {
        refresh();
    }

    void paint(juce::Graphics &) override {
    }

    void resized() override {
        auto area = getLocalBounds();
        juce::FlexBox stepsBox;
        for (size_t i = 0; i < strips.size(); i++)
            stepsBox.items.add(juce::FlexItem(*strips[i]).withHeight((float) area.getHeight()).withWidth(100));
        stepsBox.performLayout(area.toFloat());
    }

    void refreshActiveStep() {
        if (state.playing) {
            if (oldPlaying) {
                if (oldStepIndex != state.stepIndex) {
                    strips[oldStepIndex]->refreshActiveLight();
                    strips[state.stepIndex]->refreshActiveLight();
                    oldStepIndex = state.stepIndex;
                }
            } else { // start
                strips[state.stepIndex]->refreshActiveLight();
                oldPlaying = state.playing;
                oldStepIndex = state.stepIndex;
            }
        } else {
            if (oldPlaying) { // stop
                strips[oldStepIndex]->refreshActiveLight();
                oldPlaying = state.playing;
            }
        }
    }

    void refresh() {
        refreshSteps();
        size_t numSteps = strips.size();
        for (size_t i = 0; i < numSteps; i++) {
            strips[i]->refresh();
        }
    }

    void refreshSteps() {
        size_t oldNumSteps = strips.size();
        size_t newNumSteps = static_cast<size_t>(state.stepsParameter->getIndex() + 1);
        if (newNumSteps > oldNumSteps) {
            for (size_t i = oldNumSteps; i < newNumSteps; i++) {
                strips.push_back(std::unique_ptr<StepStrip>(new StepStrip(state, i)));
                addAndMakeVisible(*strips[i]);
            }
            oldNumSteps = newNumSteps;
            resized();
        } else if (newNumSteps < oldNumSteps) {
            for (size_t i = oldNumSteps - 1; i >= newNumSteps; i--) {
                removeChildComponent(strips[i].get());
                strips.pop_back();
            }
            oldNumSteps = newNumSteps;
            resized();
        }
    }

private:
    State &state;
    bool oldPlaying{false};
    size_t oldStepIndex{0};

    // vectors move items into their own storage, but components can't be copied/moved
    // but unique_ptr can be moved, so the vector assumes ownership
    std::vector<std::unique_ptr<StepStrip>> strips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrips)
};