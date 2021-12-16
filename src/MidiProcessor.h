#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>

#include "SpinLockedPosInfo.h"

class MidiProcessor {
public:
    void init(double sampleRate);

    void stopPlaying(juce::MidiBuffer &midi, int offset);

    void
    process(int numSamples, juce::MidiBuffer &midi, const juce::AudioPlayHead::CurrentPositionInfo &posInfo);

private:
    double sampleRate;

    juce::SortedSet<int> pressedNotes;
    juce::SortedSet<int> playingNotes;

    bool cycleOn;
    bool transportOn;
    size_t nextStepIndex;
    double prevPpqPos;
    double nextStepPpqPos; // if transport running, ppq from start
    int samplesUntilNextStep; // if transport not running, samples left in current note
};
