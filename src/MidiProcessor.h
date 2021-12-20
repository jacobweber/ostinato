#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>

#include "SpinLockedPosInfo.h"
#include "MidiValue.h"
#include "State.h"

class MidiProcessor {
public:
    void init(double sampleRate);

    void stopPlaying(juce::MidiBuffer &midiOut, int offset);

    void
    process(int numSamples, juce::MidiBuffer &midiIn, juce::MidiBuffer &midiOut,
            const juce::AudioPlayHead::CurrentPositionInfo &posInfo,
            State &state);

private:
    double sampleRate;

    juce::SortedSet<MidiValue> pressedNotes;
    juce::SortedSet<MidiValue> playingNotes;

    bool cycleOn;
    bool transportOn;
    size_t nextStepIndex;
    double prevPpqPos;
    double nextPpqPos; // predicted, in case we jump back in time and can't use prevPpqPos

    double releasePpqPos; // if transport running, ppq from start
    double nextStepPpqPos; // if transport running, ppq from start

    int samplesUntilRelease; // if transport not running, samples left in current note
    int samplesUntilNextStep; // if transport not running, samples left in current step
};
