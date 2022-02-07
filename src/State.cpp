#include <random>
#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "StepState.h"
#include "Constants.h"
#include "Step.h"

State::State(juce::AudioProcessorValueTreeState &p) : parameters(p) {
    stepsParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("steps"));
    voicesParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("voices"));
    rateParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rate"));
    rateTypeParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rateType"));
    modeParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("mode"));
    scaleParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("scale"));
    keyParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("key"));
    voiceMatchingParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("voiceMatching"));
    for (stepnum_t i = 0; i < constants::MAX_STEPS; i++) {
        for (voicenum_t j = 0; j < constants::MAX_VOICES; j++) {
            juce::AudioParameterBool *voiceParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                    "step" + std::to_string(i) + "_voice" + std::to_string(j)));
            stepState[i].voiceParameters[j] = voiceParameter ? voiceParameter : nullptr;
        }
        stepState[i].octaveParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter(
                "step" + std::to_string(i) + "_octave"));
        stepState[i].lengthParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter(
                "step" + std::to_string(i) + "_length"));
        stepState[i].volParameter = dynamic_cast<juce::AudioParameterFloat *> (parameters.getParameter(
                "step" + std::to_string(i) + "_volume"));
        stepState[i].tieParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                "step" + std::to_string(i) + "_tie"));
        stepState[i].powerParameter = dynamic_cast<juce::AudioParameterBool *> (parameters.getParameter(
                "step" + std::to_string(i) + "_power"));
    }
}

void State::resetToDefaults() {
    *(stepsParameter) = 3; // index
    *(voicesParameter) = 3; // index
    *(rateParameter) = 3; // index
    *(rateTypeParameter) = 0; // index
    *(modeParameter) = constants::modeChoices::Poly; // index
    *(scaleParameter) = 0; // index
    *(keyParameter) = 0; // index
    *(voiceMatchingParameter) = constants::voiceMatchingChoices::StartFromBottom; // index
    for (stepnum_t i = 0; i < constants::MAX_STEPS; i++) {
        for (voicenum_t j = 0; j < constants::MAX_VOICES; j++) {
            *(stepState[i].voiceParameters[j]) = i == j && j < 4;
        }
        *(stepState[i].octaveParameter) = constants::MAX_OCTAVES; // index; 0
        *(stepState[i].lengthParameter) = .5;
        *(stepState[i].tieParameter) = false;
        *(stepState[i].volParameter) = .5;
        *(stepState[i].powerParameter) = true;
    }
}

void State::saveToFile(juce::File file) {
    exportSettingsToXml()->writeTo(file, {});
}

juce::String State::saveToString() {
    juce::XmlElement::TextFormat format{};
    format.lineWrapLength = 999;
    format.newLineChars = "\n";
    return exportSettingsToXml()->toString(format);
}

void State::loadFromString(juce::String str) {
    importSettingsFromXml(juce::XmlDocument(str));
}

void State::loadFromFile(juce::File file) {
    importSettingsFromXml(juce::XmlDocument(file));
}

void State::randomizeParams(bool stepsAndVoices, bool rate, bool scale) {
    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_int_distribution<int> randVoiceEnabled(0, 5);

    int numOctaveChoices = constants::MAX_OCTAVES * 2 + 1;
    std::discrete_distribution<int> randOctave(numOctaveChoices, 0.0, static_cast<double>(numOctaveChoices),
                                                [](double val) { // make central octaves more likely
                                                    double weight = std::floor(val);
                                                    if (weight > constants::MAX_OCTAVES) {
                                                        weight = constants::MAX_OCTAVES * 2 - weight;
                                                    }
                                                    return std::pow(weight, 2);
                                                });

    std::uniform_real_distribution<float> randLength(0.0, 1.0);
    std::uniform_int_distribution<int> randTie(0, 10);
    std::uniform_real_distribution<float> randVolume(0.0, 1.0);
    std::uniform_int_distribution<int> randPower(0, 10);

    stepnum_t numSteps;
    voicenum_t numVoices;
    if (stepsAndVoices) {
        std::uniform_int_distribution<stepnum_t> randNumSteps(1, constants::MAX_STEPS);
        std::uniform_int_distribution<voicenum_t> randNumVoices(1, constants::MAX_VOICES);
        numSteps = randNumSteps(mt);
        numVoices = randNumVoices(mt);
        *(stepsParameter) = static_cast<int>(numSteps - 1); // index
        *(voicesParameter) = static_cast<int>(numVoices - 1); // index
    } else {
        numSteps = static_cast<stepnum_t>(stepsParameter->getIndex()) + 1;
        numVoices = static_cast<voicenum_t>(voicesParameter->getIndex()) + 1;
    }

    if (rate) {
        std::uniform_int_distribution<int> randRate(0, rateParameter->getAllValueStrings().size() - 1);
        std::uniform_int_distribution<int> randRateType(0, rateTypeParameter->getAllValueStrings().size() - 1);
        *(rateParameter) = randRate(mt); // index
        *(rateTypeParameter) = randRateType(mt); // index
    }

    std::uniform_int_distribution<voicenum_t> randMainVoice(0, numVoices - 1);

    if (scale) {
        std::uniform_int_distribution<int> randScale(0, scaleParameter->getAllValueStrings().size() - 1);
        *(scaleParameter) = randScale(mt); // index
    }

    for (stepnum_t i = 0; i < numSteps; i++) {
        voicenum_t mainVoice = randMainVoice(mt);
        for (voicenum_t j = 0; j < numVoices; j++) {
            bool enabled = j == mainVoice || randVoiceEnabled(mt) == 0;
            *(stepState[i].voiceParameters[j]) = enabled;
        }
        *(stepState[i].octaveParameter) = randOctave(mt); // index
        *(stepState[i].lengthParameter) = randLength(mt);
        *(stepState[i].tieParameter) = randTie(mt) == 0;
        *(stepState[i].volParameter) = randVolume(mt);
        *(stepState[i].powerParameter) = randPower(mt) != 0;
    }
}

std::unique_ptr<juce::XmlElement> State::exportSettingsToXml() {
    juce::XmlElement xml("ostinato");
    stepnum_t numSteps = static_cast<stepnum_t>(stepsParameter->getIndex()) + 1;
    xml.setAttribute("steps", static_cast<int>(numSteps));
    voicenum_t numVoices = static_cast<voicenum_t>(voicesParameter->getIndex()) + 1;
    xml.setAttribute("voices", static_cast<int>(numVoices));
    xml.setAttribute("rate", rateParameter->getCurrentValueAsText());
    xml.setAttribute("rateType", rateTypeParameter->getCurrentValueAsText());
    xml.setAttribute("mode", modeParameter->getCurrentValueAsText());
    xml.setAttribute("scale", scaleParameter->getCurrentValueAsText());
    xml.setAttribute("key", keyParameter->getCurrentValueAsText());
    xml.setAttribute("voiceMatching", voiceMatchingParameter->getCurrentValueAsText());
    for (stepnum_t i = 0; i < numSteps; i++) {
        juce::XmlElement* step = new juce::XmlElement("step");
        juce::String voices = "";
        for (int j = 0; j < static_cast<int>(numVoices); j++) {
            voices += stepState[i].voiceParameters[j]->get() ? "1" : "0";
        }
        step->setAttribute("voices", voices);
        step->setAttribute("octave", juce::String(-(stepState[i].octaveParameter->getIndex()) + constants::MAX_OCTAVES));
        step->setAttribute("length", stepState[i].lengthParameter->get());
        step->setAttribute("tie", stepState[i].tieParameter->get() ? "true" : "false");
        step->setAttribute("vol", stepState[i].volParameter->get());
        step->setAttribute("power", stepState[i].powerParameter->get() ? "true" : "false");
        xml.addChildElement(step);
    }
    return std::make_unique<juce::XmlElement>(xml);
}

void State::importSettingsFromXml(juce::XmlDocument xmlDoc) {
    resetToDefaults();

    std::unique_ptr<juce::XmlElement> xml(xmlDoc.getDocumentElement());
    if (xml == nullptr) return;
    if (!xml->hasTagName("ostinato")) return;

    int numSteps = std::min(static_cast<int>(constants::MAX_STEPS), xml->getIntAttribute("steps", 1));
    *(stepsParameter) = numSteps - 1; // index
    int numVoices = std::min(static_cast<int>(constants::MAX_VOICES), xml->getIntAttribute("voices", 1));
    *(voicesParameter) = numVoices - 1; // index
    *(rateParameter) = std::max(0, rateParameter->getAllValueStrings().indexOf(xml->getStringAttribute("rate")));
    *(rateTypeParameter) = std::max(0, rateTypeParameter->getAllValueStrings().indexOf(xml->getStringAttribute("rateType")));
    *(modeParameter) = std::max(0, modeParameter->getAllValueStrings().indexOf(xml->getStringAttribute("mode")));
    *(scaleParameter) = std::max(0, scaleParameter->getAllValueStrings().indexOf(xml->getStringAttribute("scale")));
    *(keyParameter) = std::max(0, keyParameter->getAllValueStrings().indexOf(xml->getStringAttribute("key")));
    *(voiceMatchingParameter) = std::max(0, voiceMatchingParameter->getAllValueStrings().indexOf(xml->getStringAttribute("voiceMatching")));
    stepnum_t _stepIndex = 0;
    for (auto* step : xml->getChildIterator()) {
        if (!step->hasTagName("step")) continue;
        juce::String voicesStr = step->getStringAttribute("voices", "0");
        for (int voice = 0; voice < std::min(voicesStr.length(), numVoices); voice++) {
            *(stepState[_stepIndex].voiceParameters[static_cast<size_t>(voice)]) = voicesStr[voice] == '1';
        }
        *(stepState[_stepIndex].octaveParameter) = -(step->getIntAttribute("octave", 0)) + constants::MAX_OCTAVES; // index
        *(stepState[_stepIndex].lengthParameter) = static_cast<float>(step->getDoubleAttribute("length", 0.0));
        *(stepState[_stepIndex].tieParameter) = step->getBoolAttribute("tie", false);
        *(stepState[_stepIndex].volParameter) = static_cast<float>(step->getDoubleAttribute("vol", 0.0));
        *(stepState[_stepIndex].powerParameter) = step->getBoolAttribute("power", true);
        _stepIndex++;
    }
}