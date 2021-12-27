#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "Props.h"
#include "ParametersFactory.h"
#include "MidiProcessor.h"
#include "TestAudioProcessor.h"

class MidiProcessorTester {
public:
    MidiProcessorTester() : MidiProcessorTester(100, 1000) {
    }

    MidiProcessorTester(int bs, int sr) : blockSize(bs), sampleRate(sr) {
        mp.init(sampleRate);

        *(state.stretchParameter) = false;
        *(state.stepsParameter) = 3; // index
        *(state.voicesParameter) = 3; // index
        *(state.rateParameter) = 2; // index
        *(state.rateTypeParameter) = 0; // index
        for (size_t i = 0; i < props::MAX_STEPS; i++) {
            for (size_t j = 0; j < props::MAX_VOICES; j++) {
                *(state.stepState[i].voiceParameters[j]) = false;
            }
            *(state.stepState[i].octaveParameter) = props::MAX_OCTAVES; // index; 0
            *(state.stepState[i].lengthParameter) = .5;
            *(state.stepState[i].tieParameter) = false;
            *(state.stepState[i].volParameter) = .5;
            *(state.stepState[i].powerParameter) = true;
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

        lastBlockStartSample = 0;
    }

    void processBlocks(int numBlocks) {
        double ppqPerBlock = blockSize * posInfo.bpm / 60 / sampleRate;

        juce::MidiBuffer tempMidiIn{};
        juce::MidiBuffer tempMidiOut{};
        midiOut.clear();
        for (int i = 0; i < numBlocks; i++) {
            tempMidiIn.clear();
            tempMidiOut.clear();

            for (const auto metadata: midiIn) {
                if (metadata.samplePosition < lastBlockStartSample + blockSize) {
                    tempMidiIn.addEvent(metadata.getMessage(), lastBlockStartSample + metadata.samplePosition);
                }
            }

            DBG("-- block " << i << " at " << lastBlockStartSample << " samples, " << posInfo.ppqPosition << " ppq --");
            mp.process(blockSize, tempMidiIn, tempMidiOut, posInfo, state);

            for (const auto metadata: tempMidiOut) {
                midiOut.addEvent(metadata.getMessage(), lastBlockStartSample + metadata.samplePosition);
            }

            lastBlockStartSample += blockSize;
            if (posInfo.isPlaying || posInfo.isRecording) {
                posInfo.ppqPosition += ppqPerBlock;
            }
        }
    }

    juce::String midiOutString(bool notesOnly) {
        juce::String out = "";
        for (const auto metadata: midiOut) {
            if (!notesOnly || metadata.getMessage().isNoteOn()) {
                out << metadata.samplePosition << ": "
                    << metadata.getMessage().getDescription()
                    << "\n";
            }
        }
        return out;
    }

public:
    int blockSize = 200;
    int sampleRate = 1000;
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};
    juce::AudioPlayHead::CurrentPositionInfo posInfo{};
    juce::MidiBuffer midiIn{};
    juce::MidiBuffer midiOut{};
    MidiProcessor mp{};
    int lastBlockStartSample;
};
