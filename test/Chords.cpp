#include <catch2/catch_test_macros.hpp>

#include "MidiProcessorTester.h"

TEST_CASE("Chords") {
    MidiProcessorTester tester{};

    SECTION("play chord") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Chord;
        *(tester.state.chordScaleParameter) = constants::chordScaleChoices::CSMajor;
        *(tester.state.chordVoicingParameter) = constants::voicingChoices::Triad;
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
                                               "350: Note on F3 Velocity 100 Channel 1\n"
                                               "475: Note off F3 Velocity 0 Channel 1\n"
                                               "600: Note on A3 Velocity 100 Channel 1\n"
                                               "725: Note off A3 Velocity 0 Channel 1\n"
                                               "850: Note on D4 Velocity 100 Channel 1\n"
                                               "975: Note off D4 Velocity 0 Channel 1\n");
    }

    SECTION("play chord at pressed key") {
        *(tester.state.stepsParameter) = 3; // 4 steps
        *(tester.state.voicesParameter) = 3; // 4 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Chord;
        *(tester.state.chordScaleParameter) = constants::chordScaleChoices::CSMajor;
        *(tester.state.chordVoicingParameter) = constants::voicingChoices::Power;
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
                                               "350: Note on G#3 Velocity 100 Channel 1\n"
                                               "475: Note off G#3 Velocity 0 Channel 1\n"
                                               "600: Note on C#4 Velocity 100 Channel 1\n"
                                               "725: Note off C#4 Velocity 0 Channel 1\n"
                                               "850: Note on G#4 Velocity 100 Channel 1\n"
                                               "975: Note off G#4 Velocity 0 Channel 1\n");
    }

    SECTION("play multi-octave chord") {
        *(tester.state.stepsParameter) = 6; // 7 steps
        *(tester.state.voicesParameter) = 6; // 7 voices
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.modeParameter) = constants::modeChoices::Chord;
        *(tester.state.chordScaleParameter) = constants::chordScaleChoices::CSMinorHarmonic;
        *(tester.state.chordVoicingParameter) = constants::voicingChoices::Ninth;
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
                                               "350: Note on G#3 Velocity 100 Channel 1\n"
                                               "475: Note off G#3 Velocity 0 Channel 1\n"
                                               "600: Note on C4 Velocity 100 Channel 1\n"
                                               "725: Note off C4 Velocity 0 Channel 1\n"
                                               "850: Note on D#4 Velocity 100 Channel 1\n"
                                               "975: Note off D#4 Velocity 0 Channel 1\n"
                                               "1100: Note on G4 Velocity 100 Channel 1\n"
                                               "1225: Note off G4 Velocity 0 Channel 1\n"
                                               "1350: Note on F5 Velocity 100 Channel 1\n"
                                               "1475: Note off F5 Velocity 0 Channel 1\n");
    }
}