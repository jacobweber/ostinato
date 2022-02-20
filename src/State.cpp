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
    chordScaleParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("chordScale"));
    chordVoicingParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("chordVoicing"));
    keyParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("key"));
    voiceMatchingParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("voiceMatching"));
    for (size_t i = 0; i < static_cast<size_t>(constants::MAX_STEPS); i++) {
        for (size_t j = 0; j < static_cast<size_t>(constants::MAX_VOICES); j++) {
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

    juce::PropertiesFile::Options options;
    options.applicationName = JucePlugin_Name;
    options.filenameSuffix = ".xml";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = JucePlugin_Name;
    props.setStorageParameters(options);
}

void State::resetToDefaults() {
    stepsParameter->beginChangeGesture();
    *(stepsParameter) = 3; // index
    stepsParameter->endChangeGesture();

    voicesParameter->beginChangeGesture();
    *(voicesParameter) = 3; // index
    voicesParameter->endChangeGesture();

    rateParameter->beginChangeGesture();
    *(rateParameter) = 3; // index
    rateParameter->endChangeGesture();

    rateTypeParameter->beginChangeGesture();
    *(rateTypeParameter) = 0; // index
    rateTypeParameter->endChangeGesture();

    modeParameter->beginChangeGesture();
    *(modeParameter) = constants::modeChoices::Poly; // index
    modeParameter->endChangeGesture();

    scaleParameter->beginChangeGesture();
    *(scaleParameter) = 0; // index
    scaleParameter->endChangeGesture();

    chordScaleParameter->beginChangeGesture();
    *(chordScaleParameter) = 0; // index
    chordScaleParameter->endChangeGesture();

    chordVoicingParameter->beginChangeGesture();
    *(chordVoicingParameter) = 0; // index
    chordVoicingParameter->endChangeGesture();

    keyParameter->beginChangeGesture();
    *(keyParameter) = 0; // index
    keyParameter->endChangeGesture();

    voiceMatchingParameter->beginChangeGesture();
    *(voiceMatchingParameter) = constants::voiceMatchingChoices::StartFromBottom; // index
    voiceMatchingParameter->endChangeGesture();

    for (size_t i = 0; i < static_cast<size_t>(constants::MAX_STEPS); i++) {
        for (size_t j = 0; j < static_cast<size_t>(constants::MAX_VOICES); j++) {
            auto param = stepState[i].voiceParameters[j];
            param->beginChangeGesture();
            *param = i == j && j < 4;
            param->endChangeGesture();
        }

        stepState[i].octaveParameter->beginChangeGesture();
        *(stepState[i].octaveParameter) = constants::MAX_OCTAVES; // index; 0
        stepState[i].octaveParameter->endChangeGesture();

        stepState[i].lengthParameter->beginChangeGesture();
        *(stepState[i].lengthParameter) = .5;
        stepState[i].lengthParameter->endChangeGesture();

        stepState[i].tieParameter->beginChangeGesture();
        *(stepState[i].tieParameter) = false;
        stepState[i].tieParameter->endChangeGesture();

        stepState[i].volParameter->beginChangeGesture();
        *(stepState[i].volParameter) = .5;
        stepState[i].volParameter->endChangeGesture();

        stepState[i].powerParameter->beginChangeGesture();
        *(stepState[i].powerParameter) = true;
        stepState[i].powerParameter->endChangeGesture();
    }
}

void State::shiftStepsLeft() {
    int numSteps = stepsParameter->getIndex() + 1;
    StepState::Temp temp{};
    stepState[0].toTemp(temp);
    for (int stepNum = 0; stepNum < numSteps - 1; stepNum++) {
        stepState[static_cast<size_t>(stepNum)]
            = stepState[static_cast<size_t>(stepNum + 1)];
    }
    stepState[static_cast<size_t>(numSteps) - 1] = temp;
}

void State::shiftStepsRight() {
    int numSteps = stepsParameter->getIndex() + 1;
    StepState::Temp temp{};
    stepState[static_cast<size_t>(numSteps) - 1].toTemp(temp);
    for (int stepNum = numSteps - 1; stepNum > 0; stepNum--) {
        stepState[static_cast<size_t>(stepNum)]
            = stepState[static_cast<size_t>(stepNum - 1)];
    }
    stepState[0] = temp;
}

void State::shiftVoicesDown() {
    int numSteps = stepsParameter->getIndex() + 1;
    int numVoices = voicesParameter->getIndex() + 1;
    for (int stepNum = 0; stepNum < numSteps; stepNum++) {
        auto voiceParams = stepState[static_cast<size_t>(stepNum)].voiceParameters;
        bool temp = voiceParams[0]->get();
        for (int voiceNum = 0; voiceNum < numVoices - 1; voiceNum++) {
            auto param = voiceParams[static_cast<size_t>(voiceNum)];
            param->beginChangeGesture();
            *param = voiceParams[static_cast<size_t>(voiceNum + 1)]->get();
            param->endChangeGesture();
        }

        auto param = voiceParams[static_cast<size_t>(numVoices) - 1];
        param->beginChangeGesture();
        *param = temp;
        param->endChangeGesture();
    }
}

void State::shiftVoicesUp() {
    int numSteps = stepsParameter->getIndex() + 1;
    int numVoices = voicesParameter->getIndex() + 1;
    for (int stepNum = 0; stepNum < numSteps; stepNum++) {
        auto voiceParams = stepState[static_cast<size_t>(stepNum)].voiceParameters;
        bool temp = voiceParams[static_cast<size_t>(numVoices) - 1]->get();
        for (int voiceNum = numVoices - 1; voiceNum > 0; voiceNum--) {
            auto param = voiceParams[static_cast<size_t>(voiceNum)];
            param->beginChangeGesture();
            *param = voiceParams[static_cast<size_t>(voiceNum - 1)]->get();
            param->endChangeGesture();
        }

        voiceParams[0]->beginChangeGesture();
        *(voiceParams[0]) = temp;
        voiceParams[0]->endChangeGesture();
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
    std::discrete_distribution<int> randOctave(static_cast<size_t>(numOctaveChoices), 0.0, static_cast<double>(numOctaveChoices),
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

    int numSteps;
    int numVoices;
    if (stepsAndVoices) {
        std::uniform_int_distribution<int> randNumSteps(1, constants::MAX_STEPS);
        std::uniform_int_distribution<int> randNumVoices(1, constants::MAX_VOICES);
        numSteps = randNumSteps(mt);
        numVoices = randNumVoices(mt);

        stepsParameter->beginChangeGesture();
        *(stepsParameter) = numSteps - 1; // index
        stepsParameter->endChangeGesture();

        voicesParameter->beginChangeGesture();
        *(voicesParameter) = numVoices - 1; // index
        voicesParameter->endChangeGesture();
    } else {
        numSteps = stepsParameter->getIndex() + 1;
        numVoices = voicesParameter->getIndex() + 1;
    }

    if (rate) {
        std::uniform_int_distribution<int> randRate(0, rateParameter->getAllValueStrings().size() - 1);
        std::uniform_int_distribution<int> randRateType(0, rateTypeParameter->getAllValueStrings().size() - 1);

        rateParameter->beginChangeGesture();
        *(rateParameter) = randRate(mt); // index
        rateParameter->endChangeGesture();

        rateTypeParameter->beginChangeGesture();
        *(rateTypeParameter) = randRateType(mt); // index
        rateTypeParameter->endChangeGesture();
    }

    std::uniform_int_distribution<int> randMainVoice(0, numVoices - 1);

    if (scale) {
        std::uniform_int_distribution<int> randScale(0, scaleParameter->getAllValueStrings().size() - 1);
        scaleParameter->beginChangeGesture();
        *(scaleParameter) = randScale(mt); // index
        scaleParameter->endChangeGesture();

        std::uniform_int_distribution<int> randChordScale(0, chordScaleParameter->getAllValueStrings().size() - 1);
        chordScaleParameter->beginChangeGesture();
        *(chordScaleParameter) = randChordScale(mt); // index
        chordScaleParameter->endChangeGesture();

        std::uniform_int_distribution<int> randChordVoicing(0, chordVoicingParameter->getAllValueStrings().size() - 1);
        chordVoicingParameter->beginChangeGesture();
        *(chordVoicingParameter) = randChordVoicing(mt); // index
        chordVoicingParameter->endChangeGesture();
    }

    for (size_t i = 0; i < static_cast<size_t>(numSteps); i++) {
        int mainVoice = randMainVoice(mt);
        for (size_t j = 0; j < static_cast<size_t>(numVoices); j++) {
            bool enabled = j == static_cast<size_t>(mainVoice) || randVoiceEnabled(mt) == 0;
            auto param = stepState[i].voiceParameters[j];
            param->beginChangeGesture();
            *param = enabled;
            param->endChangeGesture();
        }

        stepState[i].octaveParameter->beginChangeGesture();
        *(stepState[i].octaveParameter) = randOctave(mt); // index
        stepState[i].octaveParameter->endChangeGesture();

        stepState[i].lengthParameter->beginChangeGesture();
        *(stepState[i].lengthParameter) = randLength(mt);
        stepState[i].lengthParameter->endChangeGesture();

        stepState[i].tieParameter->beginChangeGesture();
        *(stepState[i].tieParameter) = randTie(mt) == 0;
        stepState[i].tieParameter->endChangeGesture();

        stepState[i].volParameter->beginChangeGesture();
        *(stepState[i].volParameter) = randVolume(mt);
        stepState[i].volParameter->endChangeGesture();

        stepState[i].powerParameter->beginChangeGesture();
        *(stepState[i].powerParameter) = randPower(mt) != 0;
        stepState[i].powerParameter->endChangeGesture();
    }
}

std::unique_ptr<juce::XmlElement> State::exportSettingsToXml() {
    juce::XmlElement xml("ostinato");
    int numSteps = stepsParameter->getIndex() + 1;
    xml.setAttribute("steps", numSteps);
    int numVoices = voicesParameter->getIndex() + 1;
    xml.setAttribute("voices", numVoices);
    xml.setAttribute("rate", rateParameter->getCurrentValueAsText());
    xml.setAttribute("rateType", rateTypeParameter->getCurrentValueAsText());
    xml.setAttribute("mode", modeParameter->getCurrentValueAsText());
    xml.setAttribute("scale", scaleParameter->getCurrentValueAsText());
    xml.setAttribute("chordScale", chordScaleParameter->getCurrentValueAsText());
    xml.setAttribute("chordVoicing", chordVoicingParameter->getCurrentValueAsText());
    xml.setAttribute("key", keyParameter->getCurrentValueAsText());
    xml.setAttribute("voiceMatching", voiceMatchingParameter->getCurrentValueAsText());
    for (size_t i = 0; i < static_cast<size_t>(numSteps); i++) {
        juce::XmlElement* step = new juce::XmlElement("step");
        juce::String voices = "";
        for (size_t j = 0; j < static_cast<size_t>(numVoices); j++) {
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

    int numSteps = std::min(constants::MAX_STEPS, xml->getIntAttribute("steps", 1));
    stepsParameter->beginChangeGesture();
    *(stepsParameter) = numSteps - 1; // index
    stepsParameter->endChangeGesture();

    int numVoices = std::min(constants::MAX_VOICES, xml->getIntAttribute("voices", 1));
    voicesParameter->beginChangeGesture();
    *(voicesParameter) = numVoices - 1; // index
    voicesParameter->endChangeGesture();

    rateParameter->beginChangeGesture();
    *(rateParameter) = std::max(0, rateParameter->getAllValueStrings().indexOf(xml->getStringAttribute("rate")));
    rateParameter->endChangeGesture();

    rateTypeParameter->beginChangeGesture();
    *(rateTypeParameter) = std::max(0, rateTypeParameter->getAllValueStrings().indexOf(xml->getStringAttribute("rateType")));
    rateTypeParameter->endChangeGesture();

    modeParameter->beginChangeGesture();
    *(modeParameter) = std::max(0, modeParameter->getAllValueStrings().indexOf(xml->getStringAttribute("mode")));
    modeParameter->endChangeGesture();

    scaleParameter->beginChangeGesture();
    *(scaleParameter) = std::max(0, scaleParameter->getAllValueStrings().indexOf(xml->getStringAttribute("scale")));
    scaleParameter->endChangeGesture();

    chordScaleParameter->beginChangeGesture();
    *(chordScaleParameter) = std::max(0, chordScaleParameter->getAllValueStrings().indexOf(xml->getStringAttribute("chordScale")));
    chordScaleParameter->endChangeGesture();

    chordVoicingParameter->beginChangeGesture();
    *(chordVoicingParameter) = std::max(0, chordVoicingParameter->getAllValueStrings().indexOf(xml->getStringAttribute("chordVoicing")));
    chordVoicingParameter->endChangeGesture();

    keyParameter->beginChangeGesture();
    *(keyParameter) = std::max(0, keyParameter->getAllValueStrings().indexOf(xml->getStringAttribute("key")));
    keyParameter->endChangeGesture();

    voiceMatchingParameter->beginChangeGesture();
    *(voiceMatchingParameter) = std::max(0, voiceMatchingParameter->getAllValueStrings().indexOf(xml->getStringAttribute("voiceMatching")));
    voiceMatchingParameter->endChangeGesture();

    size_t stepNum = 0;
    for (auto* step : xml->getChildIterator()) {
        if (!step->hasTagName("step")) continue;
        juce::String voicesStr = step->getStringAttribute("voices", "0");
        for (int voice = 0; voice < std::min(voicesStr.length(), numVoices); voice++) {
            auto param = stepState[stepNum].voiceParameters[static_cast<size_t>(voice)];
            param->beginChangeGesture();
            *param = voicesStr[voice] == '1';
            param->endChangeGesture();
        }

        stepState[stepNum].octaveParameter->beginChangeGesture();
        *(stepState[stepNum].octaveParameter) = -(step->getIntAttribute("octave", 0)) + constants::MAX_OCTAVES; // index
        stepState[stepNum].octaveParameter->endChangeGesture();

        stepState[stepNum].lengthParameter->beginChangeGesture();
        *(stepState[stepNum].lengthParameter) = static_cast<float>(step->getDoubleAttribute("length", 0.0));
        stepState[stepNum].lengthParameter->endChangeGesture();

        stepState[stepNum].tieParameter->beginChangeGesture();
        *(stepState[stepNum].tieParameter) = step->getBoolAttribute("tie", false);
        stepState[stepNum].tieParameter->endChangeGesture();

        stepState[stepNum].volParameter->beginChangeGesture();
        *(stepState[stepNum].volParameter) = static_cast<float>(step->getDoubleAttribute("vol", 0.0));
        stepState[stepNum].volParameter->endChangeGesture();

        stepState[stepNum].powerParameter->beginChangeGesture();
        *(stepState[stepNum].powerParameter) = step->getBoolAttribute("power", true);
        stepState[stepNum].powerParameter->endChangeGesture();

        stepNum++;
    }
}
