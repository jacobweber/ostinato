#include <juce_audio_utils/juce_audio_utils.h>

#include "MidiProcessorTester.h"
#include "Constants.h"
#include "ParametersFactory.h"
#include "MidiProcessor.h"
#include "TestAudioProcessor.h"

MidiProcessorTester::MidiProcessorTester() : MidiProcessorTester(100, 1000) {
}

MidiProcessorTester::MidiProcessorTester(int bs, int sr) : blockSize(bs), sampleRate(sr) {
    mp.prepareToPlay(sampleRate, blockSize);

    *(state.stepsParameter) = 3; // index
    *(state.voicesParameter) = 3; // index
    *(state.rateParameter) = 2; // index
    *(state.rateTypeParameter) = 0; // index
    *(state.modeParameter) = constants::modeChoices::Poly; // index
    *(state.scaleParameter) = 0; // index
    *(state.keyParameter) = 0; // index
    *(state.voiceMatchingParameter) = constants::voiceMatchingChoices::StartFromBottom; // index
    for (stepnum_t i = 0; i < static_cast<size_t>(constants::MAX_STEPS); i++) {
        for (size_t j = 0; j < static_cast<size_t>(constants::MAX_VOICES); j++) {
            *(state.stepState[i].voiceParameters[j]) = false;
        }
        *(state.stepState[i].octaveParameter) = constants::MAX_OCTAVES; // index; 0
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

void MidiProcessorTester::processBlocks(int numBlocks) {
    double ppqPerBlock = blockSize * posInfo.bpm / 60 / sampleRate;

    juce::MidiBuffer tempMidiIn{};
    juce::MidiBuffer tempMidiOut{};
    midiOut.clear();
    for (int i = 0; i < numBlocks; i++) {
        tempMidiIn.clear();
        tempMidiOut.clear();

        for (const auto metadata: midiIn) {
            if (metadata.samplePosition >= lastBlockStartSample &&
                metadata.samplePosition < lastBlockStartSample + blockSize) {
                tempMidiIn.addEvent(metadata.getMessage(), metadata.samplePosition - lastBlockStartSample);
            }
        }

        DBG("-- block " << i << " at " << lastBlockStartSample << " samples, " << posInfo.ppqPosition << " ppq --");
        mp.process(blockSize, tempMidiIn, tempMidiOut, posInfo);

        for (const auto metadata: tempMidiOut) {
            midiOut.addEvent(metadata.getMessage(), lastBlockStartSample + metadata.samplePosition);
        }

        lastBlockStartSample += blockSize;
        if (posInfo.isPlaying || posInfo.isRecording) {
            posInfo.ppqPosition += ppqPerBlock;
        }
    }
}

juce::String MidiProcessorTester::midiOutString(bool notesOnly) {
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
