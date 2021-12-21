#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Constants.h"
#include "ParametersFactory.h"
#include "MidiProcessor.h"
#include "TestAudioProcessor.h"

class MidiProcessorTester {
public:
    MidiProcessorTester() : MidiProcessorTester(100, 1000) {
    }

    MidiProcessorTester(int bs, int sr) : bufferSize(bs), sampleRate(sr) {
        mp.init(sampleRate);

        *(state.stepsParameter) = 3; // index
        *(state.voicesParameter) = 3; // index
        *(state.rateParameter) = 2; // index
        for (size_t i = 0; i < MAX_STEPS; i++) {
            *(state.stepState[i].lengthParameter) = .5;
            for (size_t j = 0; j < MAX_VOICES; j++) {
                *(state.stepState[i].voiceParameters[j]) = false;
            }
        }

        posInfo.ppqPosition = 0;
        posInfo.ppqPositionOfLastBarStart = 0;
        posInfo.bpm = 120;
        posInfo.timeSigNumerator = 4;
        posInfo.timeSigDenominator = 4;
        posInfo.isPlaying = false;
        posInfo.isRecording = false;

        midiIn.clear();
        midiOut.clear();

        lastBlockStartSample = -1;
    }

    void processBlock() {
        midiOut.clear();
        if (lastBlockStartSample == -1) {
            lastBlockStartSample = 0;
        } else {
            lastBlockStartSample += bufferSize;
        }
        mp.process(bufferSize, midiIn, midiOut, posInfo, state);
        midiIn.clear();
    }

    void processBlocks(int numBlocks) {
        processBlocks(numBlocks, 0);
    }

    void processBlocks(int numBlocks, float ppqPerBlock) {
        blocksMidiOutString = "";
        for (int i = 0; i < numBlocks; i++) {
            midiOut.clear();
            if (lastBlockStartSample == -1) {
                lastBlockStartSample = 0;
                posInfo.ppqPosition += 0;
            } else {
                lastBlockStartSample += bufferSize;
                posInfo.ppqPosition += ppqPerBlock;
            }
            DBG("-- frame " << i << " at " << lastBlockStartSample << " samples, " << posInfo.ppqPosition << " ppq --");
            mp.process(bufferSize, midiIn, midiOut, posInfo, state);
            midiIn.clear();

            for (const auto metadata: midiOut) {
                blocksMidiOutString << (lastBlockStartSample + metadata.samplePosition) << ": "
                                    << metadata.getMessage().getDescription()
                                    << "\n";
            }
        }
    }

    juce::String midiOutString() {
        juce::String out = "";
        for (const auto metadata: midiOut) {
            out << (lastBlockStartSample + metadata.samplePosition) << ": " << metadata.getMessage().getDescription()
                << "\n";
        }
        return out;
    }

public:
    int bufferSize = 200;
    int sampleRate = 1000;
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};
    juce::AudioPlayHead::CurrentPositionInfo posInfo{};
    juce::MidiBuffer midiIn{};
    juce::MidiBuffer midiOut{};
    MidiProcessor mp{};
    int lastBlockStartSample;
    juce::String blocksMidiOutString = "";
};
