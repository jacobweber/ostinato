#include <catch2/catch.hpp>

#include "MidiProcessorTester.h"
#include "StateHelper.h"
#include "State.h"

TEST_CASE("Presets") {
    MidiProcessorTester tester{};

    SECTION("save to string") {
        *(tester.state.rateParameter) = 3; // eighths
        *(tester.state.rateTypeParameter) = 1; // triplet
        *(tester.state.voiceMatchingParameter) = constants::voiceMatchingChoices::StretchVoiceStepsPattern;
        juce::String grid = "--*--\n"
                            "-*-*-\n"
                            "*---*\n";
        StateHelper::setGrid(tester.state, grid);
        *(tester.state.stepState[0].octaveParameter) = 5; // index
        *(tester.state.stepState[1].lengthParameter) = .75f;
        *(tester.state.stepState[2].tieParameter) = true;
        *(tester.state.stepState[3].volParameter) = .25f;
        *(tester.state.stepState[4].powerParameter) = false;

        juce::String result = tester.state.saveToString();

        juce::String expected = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n"
            "<ostinato steps=\"5\" voices=\"3\" rate=\"Eighth\" rateType=\"Triplet\" mode=\"Poly\" scale=\"Major\" key=\"C\" voiceMatching=\"Stretch/shrink voice and step pattern\">\n"
            "  <step voices=\"100\" octave=\"-1\" length=\"0.5\" tie=\"false\" vol=\"0.5\" power=\"true\"/>\n"
            "  <step voices=\"010\" octave=\"0\" length=\"0.75\" tie=\"false\" vol=\"0.5\" power=\"true\"/>\n"
            "  <step voices=\"001\" octave=\"0\" length=\"0.5\" tie=\"true\" vol=\"0.5\" power=\"true\"/>\n"
            "  <step voices=\"010\" octave=\"0\" length=\"0.5\" tie=\"false\" vol=\"0.25\" power=\"true\"/>\n"
            "  <step voices=\"100\" octave=\"0\" length=\"0.5\" tie=\"false\" vol=\"0.5\" power=\"false\"/>\n"
            "</ostinato>\n";
        REQUIRE(result == expected);
    }

    SECTION("load from string") {
        juce::String input = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n"
            "<ostinato steps=\"5\" voices=\"3\" rate=\"Eighth\" rateType=\"Triplet\" notes=\"Poly\" scale=\"Major\" key=\"C\" voiceMatching=\"Stretch/shrink voice and step pattern\">\n"
            "  <step voices=\"100\" octave=\"-1\" length=\"0.5\" tie=\"false\" vol=\"0.5\" power=\"true\"/>\n"
            "  <step voices=\"010\" octave=\"0\" length=\"0.75\" tie=\"false\" vol=\"0.5\" power=\"true\"/>\n"
            "  <step voices=\"001\" octave=\"0\" length=\"0.5\" tie=\"true\" vol=\"0.5\" power=\"true\"/>\n"
            "  <step voices=\"010\" octave=\"0\" length=\"0.5\" tie=\"false\" vol=\"0.25\" power=\"true\"/>\n"
            "  <step voices=\"100\" octave=\"0\" length=\"0.5\" tie=\"false\" vol=\"0.5\" power=\"false\"/>\n"
            "</ostinato>\n";
        tester.state.loadFromString(input);

        REQUIRE(tester.state.rateParameter->getIndex() == 3); // eighths
        REQUIRE(tester.state.rateTypeParameter->getIndex() == 1); // triplet
        REQUIRE(tester.state.voiceMatchingParameter->getIndex() == constants::voiceMatchingChoices::StretchVoiceStepsPattern);
        juce::String grid = "--*--\n"
                            "-*-*-\n"
                            "*---*\n";
        REQUIRE(StateHelper::getGrid(tester.state) == grid);
        REQUIRE(tester.state.stepState[0].octaveParameter->getIndex() == 5); // index
        REQUIRE(tester.state.stepState[1].lengthParameter->get() == .75f);
        REQUIRE(tester.state.stepState[2].tieParameter->get() == true);
        REQUIRE(tester.state.stepState[3].volParameter->get() == .25f);
        REQUIRE(tester.state.stepState[4].powerParameter->get() == false);

        REQUIRE(tester.state.stepState[1].octaveParameter->getIndex() == 4); // index
        REQUIRE(tester.state.stepState[2].lengthParameter->get() == .5f);
        REQUIRE(tester.state.stepState[3].tieParameter->get() == false);
        REQUIRE(tester.state.stepState[4].volParameter->get() == .5f);
        REQUIRE(tester.state.stepState[0].powerParameter->get() == true);
    }
}