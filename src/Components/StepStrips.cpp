#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include <vector>

#include "StepStrips.h"
#include "StepStrip.h"
#include "../State.h"
#include "../Constants.h"

StepStrips::StepStrips(State &s) : state(s) {
    refresh();
}

void StepStrips::mouseDown(const juce::MouseEvent& event) {
    findMouseStepVoice(event);
    isDraggingVoices = curMouseStep != -1;
    if (isDraggingVoices) {
        juce::AudioParameterBool* param = state.stepState[static_cast<size_t>(curMouseStep)].voiceParameters[static_cast<size_t>(curMouseVoice)];
        clickTurnedVoiceOn = !(param->get());
        *param = clickTurnedVoiceOn;
        strips[static_cast<size_t>(curMouseStep)]->hoverVoice(static_cast<voicenum_t>(curMouseVoice), true);
    }
}

void StepStrips::mouseMove(const juce::MouseEvent& event) {
    // while not pressed
    int prevMouseStep = curMouseStep;
    int prevMouseVoice = curMouseVoice;
    findMouseStepVoice(event);
    if (curMouseStep != prevMouseStep || curMouseVoice != prevMouseVoice) {
        if (prevMouseStep != -1) strips[static_cast<size_t>(prevMouseStep)]->hoverVoice(static_cast<voicenum_t>(prevMouseVoice), false);
        if (curMouseStep != -1) strips[static_cast<size_t>(curMouseStep)]->hoverVoice(static_cast<voicenum_t>(curMouseVoice), true);
    }
}

void StepStrips::mouseExit(const juce::MouseEvent&) {
    if (curMouseStep != -1) strips[static_cast<size_t>(curMouseStep)]->hoverVoice(static_cast<voicenum_t>(curMouseVoice), false);
}

void StepStrips::mouseDrag(const juce::MouseEvent& event) {
    // while pressed
    if (!isDraggingVoices) return;
    int prevMouseStep = curMouseStep;
    int prevMouseVoice = curMouseVoice;
    findMouseStepVoice(event);
    if (curMouseStep != prevMouseStep || curMouseVoice != prevMouseVoice) {
        if (prevMouseStep != -1) strips[static_cast<size_t>(prevMouseStep)]->hoverVoice(static_cast<voicenum_t>(prevMouseVoice), false);
        if (curMouseStep != -1) {
            *(state.stepState[static_cast<size_t>(curMouseStep)].voiceParameters[static_cast<size_t>(curMouseVoice)]) = clickTurnedVoiceOn;
            strips[static_cast<size_t>(curMouseStep)]->hoverVoice(static_cast<voicenum_t>(curMouseVoice), true);
        }
    }
}

void StepStrips::mouseUp(const juce::MouseEvent& event) {
    findMouseStepVoice(event);
    isDraggingVoices = false;
    if (curMouseStep != -1) {
        strips[static_cast<size_t>(curMouseStep)]->hoverVoice(static_cast<voicenum_t>(curMouseVoice), true);
    }
}

void StepStrips::paint(juce::Graphics &) {
}

void StepStrips::resized() {
    auto area = getLocalBounds();
    juce::FlexBox stepsBox;
    for (auto &strip: strips)
        stepsBox.items.add(juce::FlexItem(*strip).withHeight((float) area.getHeight()).withWidth(100));
    stepsBox.performLayout(area.toFloat());
}

void StepStrips::refreshActiveStep() {
    if (state.playing) {
        if (oldPlaying) {
            if (oldStepIndex != state.stepIndex) {
                if (oldStepIndex < strips.size()) {
                    strips[oldStepIndex]->refreshActiveLight();
                }
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
            if (oldStepIndex < strips.size()) {
                strips[oldStepIndex]->refreshActiveLight();
            }
            oldPlaying = state.playing;
        }
    }
}

void StepStrips::refresh() {
    refreshSteps();
    stepnum_t numSteps = strips.size();
    for (stepnum_t i = 0; i < numSteps; i++) {
        strips[i]->refresh();
    }
    setEnabled(!state.recordButton);
}

void StepStrips::refreshSteps() {
    stepnum_t oldNumSteps = strips.size();
    auto newNumSteps = static_cast<stepnum_t>(state.stepsParameter->getIndex()) + 1;
    if (newNumSteps > oldNumSteps) {
        for (stepnum_t i = oldNumSteps; i < newNumSteps; i++) {
            strips.push_back(std::make_unique<StepStrip>(state, i));
            addAndMakeVisible(*strips[i]);
        }
        resized();
    } else if (newNumSteps < oldNumSteps) {
        for (stepnum_t i = oldNumSteps - 1; i >= newNumSteps; i--) {
            removeChildComponent(strips[i].get());
            strips.pop_back();
        }
        resized();
    }
}

bool StepStrips::findMouseStepVoice(juce::MouseEvent event) {
    juce::Point<int> pos = event.getPosition();
    curMouseStep = -1;
    curMouseVoice = -1;
    juce::Rectangle<int> firstStripBounds = strips[0]->getBounds();
    // all strips have same width and no space in between
    int mouseStep = (pos.x - firstStripBounds.getX()) / firstStripBounds.getWidth();
    if (mouseStep > -1 && mouseStep < static_cast<int>(strips.size())) {
        // voices are in same position relative to each strip, so we only need to test one
        int xRelativeToStrip = pos.x - (mouseStep * firstStripBounds.getWidth());
        curMouseVoice = strips[0]->getVoiceForPoint(xRelativeToStrip, pos.y);
        if (curMouseVoice > -1) {
            curMouseStep = mouseStep;
            return true;
        }
    }
    return false;
}
