#include <catch2/catch.hpp>

#include "MidiProcesserTester.h"

TEST_CASE("Scales") {
    MidiProcessorTester tester{};

    SECTION("play scale") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.notesParameter) = 1; // major
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 100 Channel 1\n"
                                               "475: Note off D3 Velocity 0 Channel 1\n"
                                               "600: Note on E3 Velocity 100 Channel 1\n"
                                               "725: Note off E3 Velocity 0 Channel 1\n"
                                               "850: Note on F3 Velocity 100 Channel 1\n"
                                               "975: Note off F3 Velocity 0 Channel 1\n");
    }

    SECTION("switch to scale") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.notesParameter) = 0; // as played
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 101), 9);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 102), 8);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 103), 7);

        tester.processBlocks(8);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 101 Channel 2\n"
                                               "475: Note off D3 Velocity 0 Channel 2\n"
                                               "600: Note on E3 Velocity 102 Channel 3\n"
                                               "725: Note off E3 Velocity 0 Channel 3\n");

        *(tester.state.notesParameter) = 2; // minor
        tester.processBlocks(12);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "850: Note on F3 Velocity 100 Channel 1\n"
                                               "975: Note off F3 Velocity 0 Channel 1\n"
                                               "1100: Note on C3 Velocity 100 Channel 1\n"
                                               "1225: Note off C3 Velocity 0 Channel 1\n"
                                               "1350: Note on D3 Velocity 100 Channel 1\n"
                                               "1475: Note off D3 Velocity 0 Channel 1\n"
                                               "1600: Note on D#3 Velocity 100 Channel 1\n"
                                               "1725: Note off D#3 Velocity 0 Channel 1\n"
                                               "1850: Note on F3 Velocity 100 Channel 1\n"
                                               "1975: Note off F3 Velocity 0 Channel 1\n");
    }

    SECTION("switch to scale from stretch") {
        *(tester.state.stepsParameter) = 1; // 2 steps
        *(tester.state.voicesParameter) = 1; // 2 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.notesParameter) = 0; // as played
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stretchParameter) = true;

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

        *(tester.state.notesParameter) = 1; // major
        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "1100: Note on C3 Velocity 100 Channel 1\n"
                                               "1225: Note off C3 Velocity 0 Channel 1\n"
                                               "1350: Note on D3 Velocity 100 Channel 1\n"
                                               "1475: Note off D3 Velocity 0 Channel 1\n"
                                               "1600: Note on C3 Velocity 100 Channel 1\n"
                                               "1725: Note off C3 Velocity 0 Channel 1\n"
                                               "1850: Note on D3 Velocity 100 Channel 1\n"
                                               "1975: Note off D3 Velocity 0 Channel 1\n");
    }
}