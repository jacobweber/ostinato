#include <catch2/catch.hpp>

#include "MidiProcesserTester.h"

TEST_CASE("MidiProcessor without transport") {
    MidiProcessorTester tester{};

    SECTION("one cycle") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[3]) = true;
        *(tester.state.stepState[1].voiceParameters[2]) = true;
        *(tester.state.stepState[2].voiceParameters[1]) = true;
        *(tester.state.stepState[3].voiceParameters[0]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 101 Channel 2\n"
                                               "475: Note off D3 Velocity 0 Channel 2\n"
                                               "600: Note on E3 Velocity 102 Channel 3\n"
                                               "725: Note off E3 Velocity 0 Channel 3\n"
                                               "850: Note on F3 Velocity 103 Channel 4\n"
                                               "975: Note off F3 Velocity 0 Channel 4\n");
    }

    SECTION("with stretch 2x2 to 4x4") {
        *(tester.state.stepsParameter) = 1; // 2 steps
        *(tester.state.voicesParameter) = 1; // 2 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[1]) = true;
        *(tester.state.stepState[1].voiceParameters[0]) = true;
        *(tester.state.stretchParameter) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);

        tester.processBlocks(20);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 101 Channel 2\n"
                                               "475: Note off D3 Velocity 0 Channel 2\n"
                                               "600: Note on E3 Velocity 102 Channel 3\n"
                                               "725: Note off E3 Velocity 0 Channel 3\n"
                                               "850: Note on F3 Velocity 103 Channel 4\n"
                                               "975: Note off F3 Velocity 0 Channel 4\n"
                                               "1100: Note on C3 Velocity 100 Channel 1\n"
                                               "1225: Note off C3 Velocity 0 Channel 1\n"
                                               "1350: Note on D3 Velocity 101 Channel 2\n"
                                               "1475: Note off D3 Velocity 0 Channel 2\n"
                                               "1600: Note on E3 Velocity 102 Channel 3\n"
                                               "1725: Note off E3 Velocity 0 Channel 3\n"
                                               "1850: Note on F3 Velocity 103 Channel 4\n"
                                               "1975: Note off F3 Velocity 0 Channel 4\n");
    }
}

TEST_CASE("MidiProcessor with transport") {
    MidiProcessorTester tester{};

    SECTION("one cycle") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[3]) = true;
        *(tester.state.stepState[1].voiceParameters[2]) = true;
        *(tester.state.stepState[2].voiceParameters[1]) = true;
        *(tester.state.stepState[3].voiceParameters[0]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(9);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n" // aligned to next beat
                                               "124: Note off C3 Velocity 0 Channel 1\n"
                                               "249: Note on D3 Velocity 101 Channel 2\n"
                                               "374: Note off D3 Velocity 0 Channel 2\n"
                                               "500: Note on E3 Velocity 102 Channel 3\n"
                                               "625: Note off E3 Velocity 0 Channel 3\n"
                                               "750: Note on F3 Velocity 103 Channel 4\n"
                                               "875: Note off F3 Velocity 0 Channel 4\n");
    }

    SECTION("triplets") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.rateTypeParameter) = 1; // triplet
        *(tester.state.stepState[0].voiceParameters[3]) = true;
        *(tester.state.stepState[1].voiceParameters[2]) = true;
        *(tester.state.stepState[2].voiceParameters[1]) = true;
        *(tester.state.stepState[3].voiceParameters[0]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(6);
        // 166.666... samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n" // aligned to next beat
                                               "83: Note off C3 Velocity 0 Channel 1\n"
                                               "166: Note on D3 Velocity 101 Channel 2\n"
                                               "249: Note off D3 Velocity 0 Channel 2\n"
                                               "333: Note on E3 Velocity 102 Channel 3\n"
                                               "416: Note off E3 Velocity 0 Channel 3\n"
                                               "500: Note on F3 Velocity 103 Channel 4\n"
                                               "583: Note off F3 Velocity 0 Channel 4\n");
    }

    SECTION("triplets looped") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.rateTypeParameter) = 1; // triplet
        *(tester.state.stepState[0].voiceParameters[3]) = true;
        *(tester.state.stepState[1].voiceParameters[2]) = true;
        *(tester.state.stepState[2].voiceParameters[1]) = true;
        *(tester.state.stepState[3].voiceParameters[0]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(5);
        // 166.666... samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n" // aligned to next beat
                                               "83: Note off C3 Velocity 0 Channel 1\n"
                                               "166: Note on D3 Velocity 101 Channel 2\n"
                                               "249: Note off D3 Velocity 0 Channel 2\n"
                                               "333: Note on E3 Velocity 102 Channel 3\n"
                                               "416: Note off E3 Velocity 0 Channel 3\n");
        tester.posInfo.ppqPosition = 0;
        tester.processBlocks(5);
        REQUIRE(tester.midiOutString(false) == "500: Note on F3 Velocity 103 Channel 4\n"
                                               "583: Note off F3 Velocity 0 Channel 4\n"
                                               "666: Note on C3 Velocity 100 Channel 1\n"
                                               "749: Note off C3 Velocity 0 Channel 1\n"
                                               "833: Note on D3 Velocity 101 Channel 2\n"
                                               "916: Note off D3 Velocity 0 Channel 2\n");
    }
}