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
    juce::String grid = "-*-\n"
                        "*-*\n";
    StateHelper::setGrid(state, grid);

    Stretcher str{};
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
