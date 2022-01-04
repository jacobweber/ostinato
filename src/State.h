#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <readerwriterqueue.h>

#include "StepState.h"
#include "Constants.h"
#include "Step.h"

class State {
public:
    explicit State(juce::AudioProcessorValueTreeState &p) : parameters(p) {
        stretchParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter("stretch"));
        stepsParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("steps"));
        voicesParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("voices"));
        rateParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rate"));
        rateTypeParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rateType"));
        notesParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("notes"));
        for (size_t i = 0; i < constants::MAX_STEPS; i++) {
            for (size_t j = 0; j < constants::MAX_VOICES; j++) {
                juce::AudioParameterBool *voiceParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                        "step" + std::to_string(i) + "_voice" + std::to_string(j)));
                stepState[i].voiceParameters[j] = voiceParameter ? voiceParameter : nullptr;
            }
            stepState[i].octaveParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_octave"));
            stepState[i].lengthParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_length"));
            stepState[i].volParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_volume"));
            stepState[i].tieParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_tie"));
            stepState[i].powerParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_power"));
        }
    }

public:
    juce::AudioProcessorValueTreeState &parameters;

    juce::AudioParameterBool *stretchParameter = nullptr;
    juce::AudioParameterChoice *stepsParameter = nullptr;
    juce::AudioParameterChoice *voicesParameter = nullptr;
    juce::AudioParameterChoice *rateParameter = nullptr;
    juce::AudioParameterChoice *rateTypeParameter = nullptr;
    juce::AudioParameterChoice *notesParameter = nullptr;
    std::array<StepState, constants::MAX_STEPS> stepState;

    std::atomic<size_t> stepIndex{0};
    std::atomic<bool> playing{false};
    std::atomic<bool> recordButton{false};
    std::atomic<bool> recordedRest{false};

    moodycamel::ReaderWriterQueue<UpdatedSteps> updateStepsFromAudioThread{5};
};
