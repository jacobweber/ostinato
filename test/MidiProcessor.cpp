#include <catch2/catch.hpp>

#include "MidiProcessor.h"
#include "TestAudioProcessor.h"
#include "ParametersFactory.h"

TEST_CASE("MidiProcessor")
{
    const int BLOCK_SIZE = 200;
    const int SAMPLE_RATE = 1000;

    juce::MidiBuffer midiIn{};
    juce::MidiBuffer midiOut{};
    juce::AudioPlayHead::CurrentPositionInfo posInfo{};
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};

    *(state.stepsParameter) = 4; // index + 1
    *(state.voicesParameter) = 4; // index + 1
    *(state.rateParameter) = 3; // index + 1
    for (size_t i = 0; i < 4; i++) {
        *(state.stepState[i].lengthParameter) = .5;
        for (size_t j = 0; j < 4; j++) {
            *(state.stepState[i].voiceParameters[j]) = (j == i);
        }
    }

    MidiProcessor mp{};
    mp.init(SAMPLE_RATE);

    posInfo.ppqPosition = 0;
    posInfo.ppqPositionOfLastBarStart = 0;
    posInfo.bpm = 120;
    posInfo.timeSigNumerator = 4;
    posInfo.timeSigDenominator = 4;
    posInfo.isPlaying = false;
    posInfo.isRecording = false;

    midiIn.clear();
    midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
    midiOut.clear();
    mp.process(BLOCK_SIZE, midiIn, midiOut, posInfo, state);

    juce::String out = "";
    for (const auto metadata: midiOut) {
        out += metadata.getMessage().getDescription() + "\n";
    }

    REQUIRE(out == "Note on C3 Velocity 100 Channel 1\n");
}
