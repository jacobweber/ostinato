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
    TestAudioProcessor ap{ParametersFactory::create()};
    State state{ap.state};
    Stretcher str{};
    // numSteps = 1 + (oldNumSteps - 1) * (numNotes - 1) / (oldNumVoices - 1)

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
}
