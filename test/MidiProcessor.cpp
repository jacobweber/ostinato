#include <catch2/catch.hpp>

#include "MidiProcessorTester.h"

TEST_CASE("MidiProcessor without transport") {
    MidiProcessorTester tester{};

    SECTION("one cycle") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

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
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        tester.state.voiceMatching = constants::voiceMatchingChoices::StretchVoiceStepsPattern;

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

    SECTION("with sustain pedal") {
        *(tester.state.stepsParameter) = 1; // 2 steps
        *(tester.state.voicesParameter) = 1; // 2 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);

        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 120), 200); // sustain on

        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 60), 800);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(2, 62), 800);

        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 0), 1800); // sustain off

        tester.processBlocks(20);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 101 Channel 2\n"
                                               "475: Note off D3 Velocity 0 Channel 2\n"
                                               "600: Note on C3 Velocity 100 Channel 1\n"
                                               "725: Note off C3 Velocity 0 Channel 1\n"
                                               "850: Note on D3 Velocity 101 Channel 2\n"
                                               "975: Note off D3 Velocity 0 Channel 2\n"
                                               "1100: Note on C3 Velocity 100 Channel 1\n"
                                               "1225: Note off C3 Velocity 0 Channel 1\n"
                                               "1350: Note on D3 Velocity 101 Channel 2\n"
                                               "1475: Note off D3 Velocity 0 Channel 2\n"
                                               "1600: Note on C3 Velocity 100 Channel 1\n"
                                               "1725: Note off C3 Velocity 0 Channel 1\n");
    }

    SECTION("un-stick note") {
        *(tester.state.stepsParameter) = 1; // 2 steps
        *(tester.state.voicesParameter) = 1; // 2 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 62, (juce::uint8) 101), 9);
        tester.processBlocks(2);
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n");

        // you could bypass the plugin and release the notes, so we'd never receive the note-offs.
        // when you un-bypass, the cycle will continue until you press/release the notes again.
        tester.processBlocks(2);
        REQUIRE(tester.midiOutString(false) == "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 101 Channel 1\n");

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 400);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 60), 450);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 62, (juce::uint8) 100), 410);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 62), 460);
        tester.processBlocks(4);
        // we end the cycle at the start of the block where we receive the note-offs
        REQUIRE(tester.midiOutString(false) == "400: Note off D3 Velocity 0 Channel 1\n"
                                               "450: Note off C3 Velocity 0 Channel 1\n"
                                               "460: Note off D3 Velocity 0 Channel 1\n");
    }

    SECTION("with transpose mapping") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;
        tester.state.voiceMatching = constants::voiceMatchingChoices::UseHigherOctaves;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 74, (juce::uint8) 101), 9);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D4 Velocity 101 Channel 2\n"
                                               "475: Note off D4 Velocity 0 Channel 2\n"
                                               "600: Note on C5 Velocity 100 Channel 1\n"
                                               "725: Note off C5 Velocity 0 Channel 1\n"
                                               "850: Note on D6 Velocity 101 Channel 2\n"
                                               "975: Note off D6 Velocity 0 Channel 2\n");
    }

    SECTION("with stretch voice mapping 3 to 5") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 2; // 3 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[2]) = true;
        *(tester.state.stepState[2].voiceParameters[1]) = true;
        tester.state.voiceMatching = constants::voiceMatchingChoices::StretchVoicePattern;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(5, 67, (juce::uint8) 104), 6);

        tester.processBlocks(8);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on G3 Velocity 104 Channel 5\n"
                                               "475: Note off G3 Velocity 0 Channel 5\n"
                                               "600: Note on E3 Velocity 102 Channel 3\n"
                                               "725: Note off E3 Velocity 0 Channel 3\n");
    }

    SECTION("with stretch voice mapping 5 to 3") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 4; // 5 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[4]) = true;
        *(tester.state.stepState[2].voiceParameters[3]) = true;
        *(tester.state.stepState[3].voiceParameters[2]) = true;
        tester.state.voiceMatching = constants::voiceMatchingChoices::StretchVoicePattern;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on E3 Velocity 102 Channel 3\n"
                                               "475: Note off E3 Velocity 0 Channel 3\n"
                                               "600: Note on E3 Velocity 102 Channel 3\n"
                                               "725: Note off E3 Velocity 0 Channel 3\n"
                                               "850: Note on D3 Velocity 101 Channel 2\n"
                                               "975: Note off D3 Velocity 0 Channel 2\n");
    }
}

TEST_CASE("MidiProcessor with transport") {
    MidiProcessorTester tester{};

    SECTION("one cycle") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(9);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n" // aligned to prev beat
                                               "124: Note off C3 Velocity 0 Channel 1\n"
                                               "249: Note on D3 Velocity 101 Channel 2\n"
                                               "374: Note off D3 Velocity 0 Channel 2\n"
                                               "500: Note on E3 Velocity 102 Channel 3\n"
                                               "625: Note off E3 Velocity 0 Channel 3\n"
                                               "750: Note on F3 Velocity 103 Channel 4\n"
                                               "875: Note off F3 Velocity 0 Channel 4\n");
    }

    SECTION("one cycle, late start") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        // currently we only care about the start block, not the exact start time
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 150);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 151);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 152);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 153);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(12);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "249: Note on C3 Velocity 100 Channel 1\n" // aligned to next beat
                                               "374: Note off C3 Velocity 0 Channel 1\n"
                                               "500: Note on D3 Velocity 101 Channel 2\n"
                                               "625: Note off D3 Velocity 0 Channel 2\n"
                                               "750: Note on E3 Velocity 102 Channel 3\n"
                                               "875: Note off E3 Velocity 0 Channel 3\n"
                                               "1000: Note on F3 Velocity 103 Channel 4\n"
                                               "1125: Note off F3 Velocity 0 Channel 4\n");
    }

    SECTION("triplets") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.rateTypeParameter) = 1; // triplet
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(6);
        // 166.666... samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n"
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
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(5);
        // 166.666... samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n"
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

    SECTION("two cycles, change settings") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);
        tester.posInfo.isPlaying = true;

        tester.processBlocks(9);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "0: Note on C3 Velocity 100 Channel 1\n"
                                               "124: Note off C3 Velocity 0 Channel 1\n"
                                               "249: Note on D3 Velocity 101 Channel 2\n"
                                               "374: Note off D3 Velocity 0 Channel 2\n"
                                               "500: Note on E3 Velocity 102 Channel 3\n"
                                               "625: Note off E3 Velocity 0 Channel 3\n"
                                               "750: Note on F3 Velocity 103 Channel 4\n"
                                               "875: Note off F3 Velocity 0 Channel 4\n");

        *(tester.state.rateParameter) = 2; // quarters
        *(tester.state.stepState[0].voiceParameters[3]) = true;
        tester.processBlocks(9);
        // 500 samples/step
        REQUIRE(tester.midiOutString(false) == "1000: Note on C3 Velocity 100 Channel 1\n"
                                               "1000: Note on F3 Velocity 103 Channel 4\n"
                                               "1250: Note off C3 Velocity 0 Channel 1\n"
                                               "1250: Note off F3 Velocity 0 Channel 4\n"
                                               "1499: Note on D3 Velocity 101 Channel 2\n"
                                               "1749: Note off D3 Velocity 0 Channel 2\n");
    }
}