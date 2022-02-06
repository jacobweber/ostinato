#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "Constants.h"
#include "MidiValue.h"
#include "Step.h"

class Recorder {
public:
    enum Status {
        Active, RanOutOfSteps, Inactive
    };
    typedef std::array<juce::SortedSet<MidiValue>, constants::MAX_STEPS> NotesInSteps;

    explicit Recorder(State &_state);

    void prepareToPlay(double _sampleRate, int);

    Status getStatus() const;

    void resetStatus();

    void handleRecordButtonOn();

    void handleRecordButtonOff();

    void insertRest();

    void process(int numSamples, juce::MidiBuffer &midiIn, juce::MidiBuffer &midiOut,
                 const juce::AudioPlayHead::CurrentPositionInfo &);

private:
    void finalizeStep(bool allowEmpty);

    UpdatedSteps getUpdatedSteps();

private:
    State &state;
    double sampleRate = 0.0;
    int maxSamplesBetweenSteps = 0;

    Recorder::Status recording = Inactive;

    juce::SortedSet<MidiValue> notesInCurrentStep;
    int samplesUntilStepFinalized = -1;

    NotesInSteps notesInSteps;
    stepnum_t numSteps = 0;

    std::atomic<bool> refreshUI = false;
};
