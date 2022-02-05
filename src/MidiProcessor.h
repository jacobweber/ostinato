#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>

#include "SpinLockedPosInfo.h"
#include "MidiValue.h"
#include "State.h"
#include "Stretcher.h"
#include "Recorder.h"
#include "Step.h"
#include "Scales.h"
#include "Constants.h"

class MidiProcessor {
public:
    explicit MidiProcessor(State &s) : state(s) {
    }

    void prepareToPlay(double sampleRate, int);

    void
    process(int numSamples, juce::MidiBuffer &midiIn, juce::MidiBuffer &midiOut,
            const juce::AudioPlayHead::CurrentPositionInfo &posInfo);

private:
    void stopPlaying(juce::MidiBuffer &midiOut, int offset);

    void getCurrentStep(voicenum_t &outNumVoices);

    void playCurrentStep(voicenum_t numVoices, juce::MidiBuffer &midiOut, int playSampleOffsetWithinBlock);

    static double getPpqPosPerStep(State &state);

    static double roundStartPpqPos(double scheduledPpqPos, double ppqPosPerStep);

    static double roundNextPpqPos(double scheduledPpqPos, double ppqPosPerStep);

private:
    State &state;
    double sampleRate = 0.0;

    juce::SortedSet<MidiValue> pressedNotes;
    juce::SortedSet<MidiValue> playingNotes;

    bool cycleOn = false;
    bool transportOn = false;
    bool sustainOn = false;
    stepnum_t nextStepIndex = 0;
    double prevPpqPos = 0.0;
    double nextPpqPos = 0.0; // predicted, in case we jump back in time
    bool tieActive = false;

    double releasePpqPos = 0.0; // if transport running, ppq from start
    double nextStepPpqPos = 0.0; // if transport running, ppq from start

    int samplesUntilRelease = 0; // if transport not running, samples left in current note
    int samplesUntilNextStep = 0; // if transport not running, samples left in current step

    CurrentStep currentStep{};

    bool stretchStepsActive = false;
    Stretcher stretcher{state};

    Recorder recorder{state};

    Scales scales;
};
