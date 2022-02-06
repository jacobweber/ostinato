#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"
#include "ParametersFactory.h"
#include "MidiProcessor.h"
#include "TestAudioProcessor.h"

class MidiProcessorTester {
public:
    MidiProcessorTester();

    MidiProcessorTester(int bs, int sr);

    void processBlocks(int numBlocks);

    juce::String midiOutString(bool notesOnly);

public:
    int blockSize = 100;
    int sampleRate = 1000;
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};
    juce::AudioPlayHead::CurrentPositionInfo posInfo{};
    juce::MidiBuffer midiIn{};
    juce::MidiBuffer midiOut{};
    MidiProcessor mp{state};
    int lastBlockStartSample;
};
