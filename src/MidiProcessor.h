#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>

class MidiProcessor
{
public:
    void init(double sampleRate);

    void process(int numSamples, juce::MidiBuffer &midi, float speed);

private:
    int currentNote, lastNoteValue;
    int time;
    float rate;
    juce::SortedSet<int> notes;
};
