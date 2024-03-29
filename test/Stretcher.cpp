#include <catch2/catch_test_macros.hpp>

#include "ParametersFactory.h"
#include "TestAudioProcessor.h"
#include "Stretcher.h"
#include "StateHelper.h"

TEST_CASE("Stretcher")
{
    DBG("\n---------");
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};
    Stretcher str{state};
    str.setSkipLastStepIfMatchesFirst(false);
    // numSteps = 1 + (origNumSteps - 1) * (numNotes - 1) / (origNumVoices - 1)

    SECTION("2x2 to 4x4") {
        juce::String grid = "-*\n"
                            "*-\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(4);
        REQUIRE(result.numSteps == 4);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "---*\n"
                                    "--*-\n"
                                    "-*--\n"
                                    "*---\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("2x2 to 4x4 repeat") {
        juce::String grid = "-*\n"
                            "*-\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(4, 8);
        REQUIRE(result.numSteps == 4);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "---*---*\n"
                                    "--*---*-\n"
                                    "-*---*--\n"
                                    "*---*---\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("2x2 to 4x4 skip last") {
        str.setSkipLastStepIfMatchesFirst(true);
        juce::String grid = "-*\n"
                            "*-\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(4, 8);
        REQUIRE(result.numSteps == 4);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "---*---*\n"
                                    "--*---*-\n"
                                    "-*---*--\n"
                                    "*---*---\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("3x2 to 9x5") {
        juce::String grid = "-*-\n"
                            "*-*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(5);
        REQUIRE(result.numSteps == 9);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "----*----\n"
                                    "---*-*---\n"
                                    "--*---*--\n"
                                    "-*-----*-\n"
                                    "*-------*\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("3x2 to 9x5 skip last") {
        str.setSkipLastStepIfMatchesFirst(true);
        juce::String grid = "-*-\n"
                            "*-*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(5, 13);
        REQUIRE(result.numSteps == 8);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "----*-------*\n"
                                    "---*-*-----*-\n"
                                    "--*---*---*--\n"
                                    "-*-----*-*---\n"
                                    "*-------*----\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("3x2 to 9x5 change midstream affects skip last") {
        str.setSkipLastStepIfMatchesFirst(true);
        juce::String grid = "-*-\n"
                            "*-*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(5, 4);
        REQUIRE(result.numSteps == 8);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "----\n"
                                    "---*\n"
                                    "--*-\n"
                                    "-*--\n"
                                    "*---\n";
        REQUIRE(actualGrid == expectedGrid);

        *(state.stepState[2].voiceParameters[0]) = false;
        *(state.stepState[2].voiceParameters[1]) = true;

        result = str.stretch(5, 10);
        REQUIRE(result.numSteps == 9);

        actualGrid = StateHelper::getGrid(result);
        expectedGrid = "*****----*\n"
                       "--------*-\n"
                       "-------*--\n"
                       "------*---\n"
                       "-----*----\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("5x3 to 9x5") {
        juce::String grid = "--*--\n"
                            "-*-*-\n"
                            "*---*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(5);
        REQUIRE(result.numSteps == 9);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "----*----\n"
                                    "---*-*---\n"
                                    "--*---*--\n"
                                    "-*-----*-\n"
                                    "*-------*\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("9x5 to 3x2")
    {
        juce::String grid = "----*----\n"
                            "---*-*---\n"
                            "--*---*--\n"
                            "-*-----*-\n"
                            "*-------*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(2);
        REQUIRE(result.numSteps == 3);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "-*-\n"
                                    "*-*\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("4x2 to 16x6")
    {
        juce::String grid = "-*-*\n"
                            "*-*-\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(6);
        REQUIRE(result.numSteps == 16);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "-----*---------*\n"
                                    "----*-*-------*-\n"
                                    "---*---*-----*--\n"
                                    "--*-----*---*---\n"
                                    "-*-------*-*----\n"
                                    "*---------*-----\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("4x3 to 10x7") {
        juce::String grid = "--*-\n"
                            "-*--\n"
                            "*--*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(7);
        REQUIRE(result.numSteps == 10);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "------*---\n"
                                    "-----*----\n"
                                    "----*--*--\n"
                                    "---*------\n"
                                    "--*-----*-\n"
                                    "-*--------\n"
                                    "*--------*\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("rounded number of steps") {
        SECTION("4x3 to 6x4") {
            juce::String grid = "--*-\n"
                                "-*-*\n"
                                "*---\n";
            StateHelper::setGrid(state, grid);

            Stretcher::StretchedResult result = str.stretch(4);
            REQUIRE(result.numSteps == 6);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "---*--\n"
                                        "--*-**\n" // good?
                                        "-*----\n"
                                        "*-----\n";
            REQUIRE(actualGrid == expectedGrid);
        }

        SECTION("6x4 to 10x6") {
            juce::String grid = "---***\n"
                                "--*---\n"
                                "-*----\n"
                                "*-----\n";
            StateHelper::setGrid(state, grid);

            Stretcher::StretchedResult result = str.stretch(6);
            REQUIRE(result.numSteps == 10);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "-----*****\n"
                                        "----*-----\n"
                                        "---*------\n"
                                        "--*-------\n"
                                        "-*--------\n"
                                        "*---------\n";
            REQUIRE(actualGrid == expectedGrid);
        }
    }

    SECTION("rounded steps") {
        SECTION("4x4 to 5x5") {
            juce::String grid = "-*-*\n"
                                "----\n"
                                "----\n"
                                "*-*-\n";
            StateHelper::setGrid(state, grid);

            Stretcher::StretchedResult result = str.stretch(5);
            REQUIRE(result.numSteps == 5);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "----*\n" // strange but valid
                                        "-*---\n"
                                        "--*--\n"
                                        "---*-\n"
                                        "*----\n";
            REQUIRE(actualGrid == expectedGrid);
        }
    }

    SECTION("multiple voices") {
        SECTION("3x3 to 7x7") {
            juce::String grid = "-*-\n"
                                "***\n"
                                "*-*\n";
            StateHelper::setGrid(state, grid);

            Stretcher::StretchedResult result = str.stretch(7);
            REQUIRE(result.numSteps == 7);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "---*---\n"
                                        "--*-*--\n"
                                        "-*---*-\n"
                                        "*--*--*\n"
                                        "--*-*--\n"
                                        "-*---*-\n"
                                        "*-----*\n";
            REQUIRE(actualGrid == expectedGrid);
        }
    }

    SECTION("ties") {
        SECTION("4x3 to 10x7") {
            juce::String grid = "--*-\n"
                                "-*--\n"
                                "*--*\n";
            StateHelper::setGrid(state, grid);
            *(state.stepState[1].tieParameter) = true;

            Stretcher::StretchedResult result = str.stretch(7);
            REQUIRE(result.numSteps == 10);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "----------\n"
                                        "----------\n"
                                        "----------\n"
                                        "---****---\n"
                                        "--*----*--\n"
                                        "-*------*-\n"
                                        "*--------*\n";
            REQUIRE(actualGrid == expectedGrid);
        }

        SECTION("3x3 to 7x7 repeat") {
            juce::String grid = "--*\n"
                                "-*-\n"
                                "*--\n";
            StateHelper::setGrid(state, grid);
            *(state.stepState[2].tieParameter) = true;

            Stretcher::StretchedResult result = str.stretch(7, 14);
            REQUIRE(result.numSteps == 7);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "------**-----*\n"
                                        "-----*--*---*-\n"
                                        "----*----*-*--\n"
                                        "---*------*---\n"
                                        "--*-----------\n"
                                        "-*------------\n"
                                        "*-------------\n";
            REQUIRE(actualGrid == expectedGrid);
        }
    }

    SECTION("rests") {
        SECTION("4x3 to 10x7 with voices off") {
            juce::String grid = "----\n"
                                "-*--\n"
                                "*--*\n";
            StateHelper::setGrid(state, grid);

            Stretcher::StretchedResult result = str.stretch(7);
            REQUIRE(result.numSteps == 10);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "----------\n"
                                        "----------\n"
                                        "----------\n"
                                        "---***----\n"
                                        "--*-------\n"
                                        "-*--------\n"
                                        "*--------*\n";
            REQUIRE(actualGrid == expectedGrid);
        }

        SECTION("4x3 to 10x7 with power off") {
            juce::String grid = "--*-\n"
                                "-*--\n"
                                "*--*\n";
            StateHelper::setGrid(state, grid);
            *(state.stepState[2].powerParameter) = false;

            Stretcher::StretchedResult result = str.stretch(7);
            REQUIRE(result.numSteps == 10);

            juce::String actualGrid = StateHelper::getGrid(result);
            juce::String expectedGrid = "----------\n"
                                        "----------\n"
                                        "----------\n"
                                        "---***----\n"
                                        "--*-------\n"
                                        "-*--------\n"
                                        "*--------*\n";
            REQUIRE(actualGrid == expectedGrid);
        }
    }

    SECTION("volume") {
        SECTION("3x2 to 9x5") {
            juce::String grid = "-*-\n"
                                "*-*\n";
            StateHelper::setGrid(state, grid);
            *(state.stepState[0].volParameter) = 0.0;
            *(state.stepState[1].volParameter) = 1.0;
            *(state.stepState[2].volParameter) = 0.0;

            Stretcher::StretchedResult result = str.stretch(5);

            REQUIRE(result.steps[0].volume == 0.00);
            REQUIRE(result.steps[1].volume == 0.25);
            REQUIRE(result.steps[2].volume == 0.50);
            REQUIRE(result.steps[3].volume == 0.75);
            REQUIRE(result.steps[4].volume == 1.00);
            REQUIRE(result.steps[5].volume == 0.75);
            REQUIRE(result.steps[6].volume == 0.50);
            REQUIRE(result.steps[7].volume == 0.25);
            REQUIRE(result.steps[8].volume == 0.00);
        }
    }

    SECTION("length") {
        SECTION("3x2 to 9x5") {
            juce::String grid = "-*-\n"
                                "*-*\n";
            StateHelper::setGrid(state, grid);
            *(state.stepState[0].lengthParameter) = 0.0;
            *(state.stepState[1].lengthParameter) = 1.0;
            *(state.stepState[2].lengthParameter) = 0.0;

            Stretcher::StretchedResult result = str.stretch(5);

            REQUIRE(result.steps[0].length == 0.00);
            REQUIRE(result.steps[1].length == 0.25);
            REQUIRE(result.steps[2].length == 0.50);
            REQUIRE(result.steps[3].length == 0.75);
            REQUIRE(result.steps[4].length == 1.00);
            REQUIRE(result.steps[5].length == 0.75);
            REQUIRE(result.steps[6].length == 0.50);
            REQUIRE(result.steps[7].length == 0.25);
            REQUIRE(result.steps[8].length == 0.00);
        }
    }

    SECTION("octave") {
        SECTION("3x2 to 9x5") {
            juce::String grid = "-*-\n"
                                "*-*\n";
            StateHelper::setGrid(state, grid);
            *(state.stepState[0].octaveParameter) = constants::MAX_OCTAVES;
            *(state.stepState[1].octaveParameter) = constants::MAX_OCTAVES - 1;
            *(state.stepState[2].octaveParameter) = constants::MAX_OCTAVES + 1;

            Stretcher::StretchedResult result = str.stretch(5);

            // makes sense, since 4 new steps span 1 old step
            REQUIRE(result.steps[0].octave == constants::MAX_OCTAVES);
            REQUIRE(result.steps[1].octave == constants::MAX_OCTAVES);
            REQUIRE(result.steps[2].octave == constants::MAX_OCTAVES);
            REQUIRE(result.steps[3].octave == constants::MAX_OCTAVES);
            REQUIRE(result.steps[4].octave == constants::MAX_OCTAVES - 1);
            REQUIRE(result.steps[5].octave == constants::MAX_OCTAVES - 1);
            REQUIRE(result.steps[6].octave == constants::MAX_OCTAVES - 1);
            REQUIRE(result.steps[7].octave == constants::MAX_OCTAVES - 1);
            REQUIRE(result.steps[8].octave == constants::MAX_OCTAVES + 1);
        }
    }
}
