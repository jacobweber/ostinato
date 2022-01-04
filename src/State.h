#pragma once

#include <random>
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

    void randomizeParams(bool stepsAndVoices) {
        std::random_device rd;
        std::mt19937 mt(rd());

        std::uniform_int_distribution<size_t> randNumSteps(1, constants::MAX_STEPS);
        std::uniform_int_distribution<size_t> randNumVoices(1, constants::MAX_VOICES);
        std::uniform_int_distribution<int> randRate(1, rateParameter->getAllValueStrings().size());
        std::uniform_int_distribution<int> randRateType(1, rateTypeParameter->getAllValueStrings().size());

        std::uniform_int_distribution<int> randVoice(0, 3);
        std::uniform_int_distribution<int> randOctave(0, static_cast<int>(constants::MAX_OCTAVES) * 2);
        std::uniform_real_distribution<float> randLength(0.0, 1.0);
        std::uniform_int_distribution<int> randTie(0, 10);
        std::uniform_real_distribution<float> randVolume(0.0, 1.0);
        std::uniform_int_distribution<int> randPower(0, 10);

        *(stretchParameter) = false;
        size_t numSteps;
        size_t numVoices;
        if (stepsAndVoices) {
            numSteps = randNumSteps(mt);
            numVoices = randNumVoices(mt);
            *(stepsParameter) = static_cast<int>(numSteps - 1); // index
            *(voicesParameter) = static_cast<int>(numVoices - 1); // index
        } else {
            numSteps = static_cast<size_t>(stepsParameter->getIndex()) + 1;
            numVoices = static_cast<size_t>(voicesParameter->getIndex()) + 1;
        }
        *(rateParameter) = randRate(mt); // index
        *(rateTypeParameter) = randRateType(mt); // index
        *(notesParameter) = 0; // index
        for (size_t i = 0; i < numSteps; i++) {
            for (size_t j = 0; j < numVoices; j++) {
                *(stepState[i].voiceParameters[j]) = randVoice(mt) == 0;
            }
            *(stepState[i].octaveParameter) = randOctave(mt); // index
            *(stepState[i].lengthParameter) = randLength(mt);
            *(stepState[i].tieParameter) = randTie(mt) == 0;
            *(stepState[i].volParameter) = randVolume(mt);
            *(stepState[i].powerParameter) = randPower(mt) != 0;
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
