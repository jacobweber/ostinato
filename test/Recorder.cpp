#include <catch2/catch.hpp>

#include "MidiProcesserTester.h"
#include "StateHelper.h"
#include "MessageReader.h"

TEST_CASE("Recorder") {
    MidiProcessorTester tester{};
    MessageReader reader(tester.state);

    SECTION("record pattern") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.notesParameter) = 1; // major
        *(tester.state.stepState[0].voiceParameters[3]) = true;
        *(tester.state.stepState[1].voiceParameters[2]) = true;
        *(tester.state.stepState[2].voiceParameters[1]) = true;
        *(tester.state.stepState[3].voiceParameters[0]) = true;
        tester.state.recordButton = true;

        // notes don't matter; only their relative position
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 50, (juce::uint8) 80), 10);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 50), 110);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 30);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 60), 100);
        tester.processBlocks(2);
        reader.timerCallback();

        juce::String actualGrid = StateHelper::getGrid(tester.state);
        juce::String expectedGrid = "*\n"
                                    "*\n";
        REQUIRE(actualGrid == expectedGrid);

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 50, (juce::uint8) 100), 300);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 50), 450);
        tester.processBlocks(3);
        reader.timerCallback();

        actualGrid = StateHelper::getGrid(tester.state);
        expectedGrid = "*-\n"
                       "**\n";
        REQUIRE(actualGrid == expectedGrid);

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 80, (juce::uint8) 110), 600);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 80), 750);
        tester.processBlocks(3);
        reader.timerCallback();

        actualGrid = StateHelper::getGrid(tester.state);
        expectedGrid = "--*\n"
                       "*--\n"
                       "**-\n";
        REQUIRE(actualGrid == expectedGrid);

        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 120), 900);
        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 0), 1050);
        tester.processBlocks(3);
        reader.timerCallback();

        actualGrid = StateHelper::getGrid(tester.state);
        expectedGrid = "--*-\n"
                       "*---\n"
                       "**--\n";
        REQUIRE(actualGrid == expectedGrid);

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 70, (juce::uint8) 120), 1200);
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 70), 1350);
        tester.processBlocks(3);
        reader.timerCallback();

        actualGrid = StateHelper::getGrid(tester.state);
        expectedGrid = "--*--\n"
                       "----*\n"
                       "*----\n"
                       "**---\n";
        REQUIRE(actualGrid == expectedGrid);

        tester.state.recordButton = false;
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 63), 1400);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(2, 62, (juce::uint8) 63), 1401);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(3, 64, (juce::uint8) 63), 1402);
        tester.midiIn.addEvent(juce::MidiMessage::noteOn(4, 65, (juce::uint8) 63), 1403);

        tester.processBlocks(13);
        // should make volume more predictable
        REQUIRE(tester.midiOutString(false) == "1500: Note on D3 Velocity 89 Channel 1\n"
                                               "1500: Note on C3 Velocity 89 Channel 1\n"
                                               "1625: Note off C3 Velocity 0 Channel 1\n"
                                               "1625: Note off D3 Velocity 0 Channel 1\n"
                                               "1750: Note on C3 Velocity 99 Channel 1\n"
                                               "1875: Note off C3 Velocity 0 Channel 1\n"
                                               "2000: Note on F3 Velocity 109 Channel 1\n"
                                               "2125: Note off F3 Velocity 0 Channel 1\n"
                                               "2500: Note on E3 Velocity 119 Channel 1\n"
                                               "2625: Note off E3 Velocity 0 Channel 1\n");
    }

    SECTION("record rests only") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.notesParameter) = 1; // major
        tester.state.recordButton = true;

        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 120), 50);
        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 0), 150);
        tester.processBlocks(2);
        reader.timerCallback();

        juce::String actualGrid = StateHelper::getGrid(tester.state);
        juce::String expectedGrid = "-\n";
        REQUIRE(actualGrid == expectedGrid);

        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 120), 300);
        tester.midiIn.addEvent(juce::MidiMessage::controllerEvent(1, 0x40, (juce::uint8) 0), 450);
        tester.processBlocks(3);
        reader.timerCallback();
        tester.state.recordButton = false;

        actualGrid = StateHelper::getGrid(tester.state);
        expectedGrid = "--\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("hold notes while recording") {
        *(tester.state.stepsParameter) = 1; // 2 steps
        *(tester.state.voicesParameter) = 1; // 2 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.stepState[0].voiceParameters[1]) = true;
        *(tester.state.stepState[1].voiceParameters[0]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 10);
        tester.processBlocks(2);
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n");

        tester.state.recordButton = true;
        tester.processBlocks(2);
        REQUIRE(tester.midiOutString(false) == "200: Note off C3 Velocity 0 Channel 1\n");

        tester.state.recordButton = false;
        tester.processBlocks(2);
        REQUIRE(tester.midiOutString(false) == "");
    }

    SECTION("run out of steps while recording, with held note") {
        tester.state.recordButton = true;
        int sampleNum = 0;
        for (stepnum_t step = 0; step < constants::MAX_STEPS - 1; step++) {
            tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), sampleNum);
            tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 60), sampleNum + 150);
            tester.processBlocks(2);
            REQUIRE(tester.midiOutString(false) == juce::String(sampleNum) + ": Note on C3 Velocity 100 Channel 1\n"
                                                   + juce::String(sampleNum + 150) +
                                                   ": Note off C3 Velocity 0 Channel 1\n");
            sampleNum += tester.blockSize * 2;
        }

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), sampleNum);
        tester.processBlocks(2);
        REQUIRE(tester.state.recordButton == false);
        sampleNum += tester.blockSize * 2;

        tester.state.recordButton = false;
        tester.midiIn.addEvent(juce::MidiMessage::noteOff(1, 60), sampleNum);
        tester.processBlocks(2);
        REQUIRE(tester.midiOutString(false) == juce::String(sampleNum) + ": Note off C3 Velocity 0 Channel 1\n");
    }
}