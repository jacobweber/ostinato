#include <catch2/catch.hpp>

#include "ParametersFactory.h"
#include "TestAudioProcessor.h"
#include "Stretcher.h"
#include "StateHelper.h"

TEST_CASE("balls into buckets", "[.]")
{
    Stretcher str{};
    for (int numBalls = 1; numBalls <= 9; numBalls++) {
        for (int numBuckets = 1; numBuckets <= 9; numBuckets++) {
            DBG(numBalls << " -> " << numBuckets << " = " << str.distribute(numBalls, numBuckets));
        }
    }
}

TEST_CASE("Stretcher")
{
    DBG("\n---------");
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};
    Stretcher str{};
    // numSteps = 1 + (origNumSteps - 1) * (numNotes - 1) / (origNumVoices - 1)

    SECTION("2x2 to 4x4") {
        juce::String grid = "-*\n"
                            "*-\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(state, 4);
        REQUIRE(result.numSteps == 4);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "---*\n"
                                    "--*-\n"
                                    "-*--\n"
                                    "*---\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("3x2 to 9x5") {
        juce::String grid = "-*-\n"
                            "*-*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(state, 5);
        REQUIRE(result.numSteps == 9);

        juce::String actualGrid = StateHelper::getGrid(result);
        juce::String expectedGrid = "----*----\n"
                                    "---*-*---\n"
                                    "--*---*--\n"
                                    "-*-----*-\n"
                                    "*-------*\n";
        REQUIRE(actualGrid == expectedGrid);
    }

    SECTION("5x3 to 9x5") {
        juce::String grid = "--*--\n"
                            "-*-*-\n"
                            "*---*\n";
        StateHelper::setGrid(state, grid);

        Stretcher::StretchedResult result = str.stretch(state, 5);
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

        Stretcher::StretchedResult result = str.stretch(state, 2);
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

        Stretcher::StretchedResult result = str.stretch(state, 6);
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

        Stretcher::StretchedResult result = str.stretch(state, 7);
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

            Stretcher::StretchedResult result = str.stretch(state, 4);
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

            Stretcher::StretchedResult result = str.stretch(state, 6);
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

            Stretcher::StretchedResult result = str.stretch(state, 5);
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

            Stretcher::StretchedResult result = str.stretch(state, 7);
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

            Stretcher::StretchedResult result = str.stretch(state, 7);
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
    }

    SECTION("rests") {
        SECTION("4x3 to 10x7 with voices off") {
            juce::String grid = "----\n"
                                "-*--\n"
                                "*--*\n";
            StateHelper::setGrid(state, grid);

            Stretcher::StretchedResult result = str.stretch(state, 7);
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

            Stretcher::StretchedResult result = str.stretch(state, 7);
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
}
