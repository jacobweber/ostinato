#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <readerwriterqueue.h>

#include "StepState.h"
#include "Constants.h"
#include "Step.h"

class State {
public:
    explicit State(juce::AudioProcessorValueTreeState &p);

    void resetToDefaults();

    void shiftStepsLeft();
    void shiftStepsRight();
    void shiftVoicesDown();
    void shiftVoicesUp();

    void saveToFile(juce::File file);

    juce::String saveToString();

    void loadFromString(juce::String str);

    void loadFromFile(juce::File file);

    void randomizeParams(bool stepsAndVoices, bool rate, bool scale);

private:
    std::unique_ptr<juce::XmlElement> exportSettingsToXml();

    void importSettingsFromXml(juce::XmlDocument xmlDoc);

public:
    juce::AudioProcessorValueTreeState &parameters;

    juce::AudioParameterChoice *stepsParameter = nullptr;
    juce::AudioParameterChoice *voicesParameter = nullptr;
    juce::AudioParameterChoice *rateParameter = nullptr;
    juce::AudioParameterChoice *rateTypeParameter = nullptr;
    juce::AudioParameterChoice *modeParameter = nullptr;
    juce::AudioParameterChoice *scaleParameter = nullptr;
    juce::AudioParameterChoice *chordScaleParameter = nullptr;
    juce::AudioParameterChoice *chordVoicingParameter = nullptr;
    juce::AudioParameterChoice *keyParameter = nullptr;
    std::array<StepState, constants::MAX_STEPS> stepState;

    std::atomic<int> voiceMatching{constants::voiceMatchingChoices::StartFromBottom};
    std::atomic<int> random{constants::randomChoices::Off};

    std::atomic<int> displayStepNum{0};
    std::atomic<bool> playing{false};
    std::atomic<bool> recordButton{false};
    std::atomic<bool> recordedRest{false};

    moodycamel::ReaderWriterQueue<RecordedSteps> recordedStepsFromAudioThread{16};
    moodycamel::ReaderWriterQueue<UpdatedStepSettings> updatedStepFromAudioThread{16};

    juce::ApplicationProperties props{};
    juce::PropertiesFile* userSettings{};
};
