#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "StepState.h"
#include "Constants.h"

class State {
public:
    State(juce::AudioProcessorValueTreeState &p) : parameters(p) {
        stepsParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("steps"));
        voicesParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("voices"));
        rateParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rate"));
        rateTypeParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rateType"));
        for (size_t i = 0; i < MAX_STEPS; i++) {
            stepState[i].lengthParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_length"));
            stepState[i].volParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_volume"));
            for (size_t j = 0; j < MAX_VOICES; j++) {
                juce::AudioParameterBool *voiceParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                        "step" + std::to_string(i) + "_voice" + std::to_string(j)));
                stepState[i].voiceParameters[j] = voiceParameter ? voiceParameter : nullptr;
            }
            stepState[i].powerParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_power"));
        }
    }

public:
    juce::AudioProcessorValueTreeState &parameters;

    juce::AudioParameterChoice *stepsParameter = nullptr;
    juce::AudioParameterChoice *voicesParameter = nullptr;
    juce::AudioParameterChoice *rateParameter = nullptr;
    juce::AudioParameterChoice *rateTypeParameter = nullptr;
    std::array<StepState, MAX_STEPS> stepState;

    std::atomic<size_t> stepIndex{0};
    std::atomic<bool> playing{false};
};
