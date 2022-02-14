#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include <vector>

#include "StepStrips.h"
#include "StepStrip.h"
#include "../State.h"
#include "../Constants.h"

StepStrips::StepStrips(State &_state) : state(_state) {
    refresh();
}

void StepStrips::mouseDown(const juce::MouseEvent& event) {
    findMouseStepVoice(event);
    isDraggingVoices = curMouseStep != -1;
    if (isDraggingVoices) {
        juce::AudioParameterBool* param = state.stepState[static_cast<size_t>(curMouseStep)].voiceParameters[static_cast<size_t>(curMouseVoice)];
        clickTurnedVoiceOn = !(param->get());
        *param = clickTurnedVoiceOn;
        strips[static_cast<size_t>(curMouseStep)]->hoverVoice(curMouseVoice, true);
    }
}

void StepStrips::mouseMove(const juce::MouseEvent& event) {
    // while not pressed
    int prevMouseStep = curMouseStep;
    int prevMouseVoice = curMouseVoice;
    findMouseStepVoice(event);
    if (curMouseStep != prevMouseStep || curMouseVoice != prevMouseVoice) {
        if (prevMouseStep != -1 && strips.size() > static_cast<size_t>(prevMouseStep)) {
            strips[static_cast<size_t>(prevMouseStep)]->hoverVoice(prevMouseVoice, false);
        }
        if (curMouseStep != -1) strips[static_cast<size_t>(curMouseStep)]->hoverVoice(curMouseVoice, true);
    }
}

void StepStrips::mouseExit(const juce::MouseEvent&) {
    if (curMouseStep != -1 && strips.size() > static_cast<size_t>(curMouseStep)) {
        strips[static_cast<size_t>(curMouseStep)]->hoverVoice(curMouseVoice, false);
    }
}

void StepStrips::mouseDrag(const juce::MouseEvent& event) {
    // while pressed
    if (!isDraggingVoices) return;
    int prevMouseStep = curMouseStep;
    int prevMouseVoice = curMouseVoice;
    findMouseStepVoice(event);
    if (curMouseStep != prevMouseStep || curMouseVoice != prevMouseVoice) {
        if (prevMouseStep != -1 && strips.size() > static_cast<size_t>(prevMouseStep)) {
            strips[static_cast<size_t>(prevMouseStep)]->hoverVoice(prevMouseVoice, false);
        }
        if (curMouseStep != -1) {
            *(state.stepState[static_cast<size_t>(curMouseStep)].voiceParameters[static_cast<size_t>(curMouseVoice)]) = clickTurnedVoiceOn;
            strips[static_cast<size_t>(curMouseStep)]->hoverVoice(curMouseVoice, true);
        }
    }
}

void StepStrips::mouseUp(const juce::MouseEvent& event) {
    findMouseStepVoice(event);
    isDraggingVoices = false;
    if (curMouseStep != -1) {
        strips[static_cast<size_t>(curMouseStep)]->hoverVoice(curMouseVoice, true);
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
            if (oldStepNum != state.stepNum) {
                if (static_cast<size_t>(oldStepNum) < strips.size()) {
                    strips[static_cast<size_t>(oldStepNum)]->refreshActiveLight();
                }
                strips[static_cast<size_t>(state.stepNum)]->refreshActiveLight();
                oldStepNum = state.stepNum;
            }
        } else { // start
            strips[static_cast<size_t>(state.stepNum)]->refreshActiveLight();
            oldPlaying = state.playing;
            oldStepNum = state.stepNum;
        }
    } else {
        if (oldPlaying) { // stop
            if (static_cast<size_t>(oldStepNum) < strips.size()) {
                strips[static_cast<size_t>(oldStepNum)]->refreshActiveLight();
            }
            oldPlaying = state.playing;
        }
    }
}

void StepStrips::refresh() {
    refreshSteps();
    size_t numSteps = strips.size();
    for (size_t i = 0; i < numSteps; i++) {
        strips[i]->refresh();
    }
    setEnabled(!state.recordButton);
}

void StepStrips::refreshSteps() {
    int oldNumSteps = static_cast<int>(strips.size());
    int newNumSteps = state.stepsParameter->getIndex() + 1;
    if (newNumSteps > oldNumSteps) {
        for (int i = oldNumSteps; i < newNumSteps; i++) {
            strips.push_back(std::make_unique<StepStrip>(state, i));
            addAndMakeVisible(*strips[static_cast<size_t>(i)]);
        }
        resized();
    } else if (newNumSteps < oldNumSteps) {
        for (int i = oldNumSteps - 1; i >= newNumSteps; i--) {
            removeChildComponent(strips[static_cast<size_t>(i)].get());
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
