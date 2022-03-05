#include <catch2/catch.hpp>

#include "MidiProcessorTester.h"

TEST_CASE("Scales") {
    MidiProcessorTester tester{};

    SECTION("play scale") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = constants::scaleChoices::Major;
        *(tester.state.keyParameter) = 1; // C
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 62, (juce::uint8) 100), 10);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on D3 Velocity 100 Channel 1\n"
                                               "225: Note off D3 Velocity 0 Channel 1\n"
                                               "350: Note on E3 Velocity 100 Channel 1\n"
                                               "475: Note off E3 Velocity 0 Channel 1\n"
                                               "600: Note on F3 Velocity 100 Channel 1\n"
                                               "725: Note off F3 Velocity 0 Channel 1\n"
                                               "850: Note on G3 Velocity 100 Channel 1\n"
                                               "975: Note off G3 Velocity 0 Channel 1\n");
    }

    SECTION("play at pressed key") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = constants::scaleChoices::MinorNatural;
        *(tester.state.keyParameter) = constants::PRESSED_KEY;
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 61, (juce::uint8) 100), 10);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C#3 Velocity 100 Channel 1\n"
                                               "225: Note off C#3 Velocity 0 Channel 1\n"
                                               "350: Note on D#3 Velocity 100 Channel 1\n"
                                               "475: Note off D#3 Velocity 0 Channel 1\n"
                                               "600: Note on E3 Velocity 100 Channel 1\n"
                                               "725: Note off E3 Velocity 0 Channel 1\n"
                                               "850: Note on F#3 Velocity 100 Channel 1\n"
                                               "975: Note off F#3 Velocity 0 Channel 1\n");
    }

    SECTION("play scale, rounding up from last degree") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = 1; // minor
        *(tester.state.keyParameter) = 1; // C
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 59, (juce::uint8) 100), 10);

        tester.processBlocks(10);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 100 Channel 1\n"
                                               "475: Note off D3 Velocity 0 Channel 1\n"
                                               "600: Note on D#3 Velocity 100 Channel 1\n"
                                               "725: Note off D#3 Velocity 0 Channel 1\n"
                                               "850: Note on F3 Velocity 100 Channel 1\n"
                                               "975: Note off F3 Velocity 0 Channel 1\n");
    }

    SECTION("switch to scale") {
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

        tester.processBlocks(8);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on C3 Velocity 100 Channel 1\n"
                                               "225: Note off C3 Velocity 0 Channel 1\n"
                                               "350: Note on D3 Velocity 101 Channel 2\n"
                                               "475: Note off D3 Velocity 0 Channel 2\n"
                                               "600: Note on E3 Velocity 102 Channel 3\n"
                                               "725: Note off E3 Velocity 0 Channel 3\n");

        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = 1; // minor
        *(tester.state.keyParameter) = 1; // C
        // still holding C as lowest note

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
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        tester.state.voiceMatching = constants::voiceMatchingChoices::StretchVoiceStepsPattern;

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

        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = constants::scaleChoices::Major;
        *(tester.state.keyParameter) = 1; // C
        // still holding C as lowest note

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

    SECTION("play 6-note scale") {
        *(tester.state.stepsParameter) = 6; // 7 steps
        *(tester.state.voicesParameter) = 6; // 7 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = constants::scaleChoices::Blues;
        *(tester.state.keyParameter) = 1; // C
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;
        *(tester.state.stepState[4].voiceParameters[4]) = true;
        *(tester.state.stepState[5].voiceParameters[5]) = true;
        *(tester.state.stepState[6].voiceParameters[6]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 64, (juce::uint8) 100), 10);

        tester.processBlocks(15);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on F3 Velocity 100 Channel 1\n"
                                               "225: Note off F3 Velocity 0 Channel 1\n"
                                               "350: Note on F#3 Velocity 100 Channel 1\n"
                                               "475: Note off F#3 Velocity 0 Channel 1\n"
                                               "600: Note on G3 Velocity 100 Channel 1\n"
                                               "725: Note off G3 Velocity 0 Channel 1\n"
                                               "850: Note on A#3 Velocity 100 Channel 1\n"
                                               "975: Note off A#3 Velocity 0 Channel 1\n"
                                               "1100: Note on C4 Velocity 100 Channel 1\n"
                                               "1225: Note off C4 Velocity 0 Channel 1\n"
                                               "1350: Note on D#4 Velocity 100 Channel 1\n"
                                               "1475: Note off D#4 Velocity 0 Channel 1\n");
    }

    SECTION("play multi-octave scale") {
        *(tester.state.stepsParameter) = 7; // 7 steps
        *(tester.state.voicesParameter) = 7; // 8 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Scale;
        *(tester.state.scaleParameter) = constants::scaleChoices::Fourth;
        *(tester.state.keyParameter) = 3; // D
        *(tester.state.stepState[0].voiceParameters[0]) = true;
        *(tester.state.stepState[1].voiceParameters[1]) = true;
        *(tester.state.stepState[2].voiceParameters[2]) = true;
        *(tester.state.stepState[3].voiceParameters[3]) = true;
        *(tester.state.stepState[4].voiceParameters[4]) = true;
        *(tester.state.stepState[5].voiceParameters[5]) = true;
        *(tester.state.stepState[6].voiceParameters[6]) = true;
        *(tester.state.stepState[7].voiceParameters[7]) = true;
        *(tester.state.stepState[8].voiceParameters[8]) = true;

        tester.midiIn.addEvent(juce::MidiMessage::noteOn(1, 64, (juce::uint8) 100), 10);

        tester.processBlocks(15);
        // 250 samples/step
        REQUIRE(tester.midiOutString(false) == "100: Note on G3 Velocity 100 Channel 1\n"
                                               "225: Note off G3 Velocity 0 Channel 1\n"
                                               "350: Note on C4 Velocity 100 Channel 1\n"
                                               "475: Note off C4 Velocity 0 Channel 1\n"
                                               "600: Note on F4 Velocity 100 Channel 1\n"
                                               "725: Note off F4 Velocity 0 Channel 1\n"
                                               "850: Note on A#4 Velocity 100 Channel 1\n"
                                               "975: Note off A#4 Velocity 0 Channel 1\n"
                                               "1100: Note on D#5 Velocity 100 Channel 1\n"
                                               "1225: Note off D#5 Velocity 0 Channel 1\n"
                                               "1350: Note on G#5 Velocity 100 Channel 1\n"
                                               "1475: Note off G#5 Velocity 0 Channel 1\n");
    }
}