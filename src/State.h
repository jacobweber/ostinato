#pragma once

#include <random>
#include <juce_audio_utils/juce_audio_utils.h>
#include <readerwriterqueue.h>

#include "StepState.h"
#include "Constants.h"
#include "Step.h"

class State {
public:
    explicit State(juce::AudioProcessorValueTreeState &p) : parameters(p) {
        stepsParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("steps"));
        voicesParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("voices"));
        rateParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rate"));
        rateTypeParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("rateType"));
        notesParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("notes"));
        modeParameter = dynamic_cast<juce::AudioParameterChoice *> (parameters.getParameter("mode"));
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

    void resetToDefaults() {
        *(stepsParameter) = 3; // index
        *(voicesParameter) = 3; // index
        *(rateParameter) = 3; // index
        *(rateTypeParameter) = 0; // index
        *(notesParameter) = 0; // index
        *(modeParameter) = constants::modeChoices::Poly; // index
        *(voiceMatchingParameter) = 0; // index
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

    void saveToFile(juce::File file) {
        exportSettingsToXml()->writeTo(file, {});
    }

    juce::String saveToString() {
        juce::XmlElement::TextFormat format{};
        format.lineWrapLength = 999;
        format.newLineChars = "\n";
        return exportSettingsToXml()->toString(format);
    }

    void loadFromString(juce::String str) {
        importSettingsFromXml(juce::XmlDocument(str));
    }

    void loadFromFile(juce::File file) {
        importSettingsFromXml(juce::XmlDocument(file));
    }

    void randomizeParams(bool stepsAndVoices, bool rate, bool notes) {
        std::random_device rd;
        std::mt19937 mt(rd());

        std::uniform_int_distribution<int> randVoiceEnabled(0, 5);

        size_t numOctaveChoices = constants::MAX_OCTAVES * 2 + 1;
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

        if (notes) {
            std::uniform_int_distribution<int> randNotes(0, notesParameter->getAllValueStrings().size() - 1);
            *(notesParameter) = randNotes(mt); // index
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

private:
    std::unique_ptr<juce::XmlElement> exportSettingsToXml() {
        juce::XmlElement xml("ostinato");
        stepnum_t numSteps = static_cast<stepnum_t>(stepsParameter->getIndex()) + 1;
        xml.setAttribute("steps", static_cast<int>(numSteps));
        voicenum_t numVoices = static_cast<voicenum_t>(voicesParameter->getIndex()) + 1;
        xml.setAttribute("voices", static_cast<int>(numVoices));
        xml.setAttribute("rate", rateParameter->getCurrentValueAsText());
        xml.setAttribute("rateType", rateTypeParameter->getCurrentValueAsText());
        xml.setAttribute("notes", notesParameter->getCurrentValueAsText());
        xml.setAttribute("mode", modeParameter->getCurrentValueAsText());
        xml.setAttribute("voiceMatching", voiceMatchingParameter->getCurrentValueAsText());
        for (stepnum_t i = 0; i < numSteps; i++) {
            juce::XmlElement* step = new juce::XmlElement("step");
            juce::String voices = "";
            for (int j = 0; j < static_cast<int>(numVoices); j++) {
                voices += stepState[i].voiceParameters[j]->get() ? "1" : "0";
            }
            step->setAttribute("voices", voices);
            step->setAttribute("octave", juce::String(-(stepState[i].octaveParameter->getIndex()) + static_cast<int>(constants::MAX_OCTAVES)));
            step->setAttribute("length", stepState[i].lengthParameter->get());
            step->setAttribute("tie", stepState[i].tieParameter->get() ? "true" : "false");
            step->setAttribute("vol", stepState[i].volParameter->get());
            step->setAttribute("power", stepState[i].powerParameter->get() ? "true" : "false");
            xml.addChildElement(step);
        }
        return std::make_unique<juce::XmlElement>(xml);
    }

    void importSettingsFromXml(juce::XmlDocument xmlDoc) {
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
        *(notesParameter) = std::max(0, notesParameter->getAllValueStrings().indexOf(xml->getStringAttribute("notes")));
        *(modeParameter) = std::max(0, modeParameter->getAllValueStrings().indexOf(xml->getStringAttribute("mode")));
        *(voiceMatchingParameter) = std::max(0, voiceMatchingParameter->getAllValueStrings().indexOf(xml->getStringAttribute("voiceMatching")));
        stepnum_t _stepIndex = 0;
        for (auto* step : xml->getChildIterator()) {
            if (!step->hasTagName("step")) continue;
            juce::String voicesStr = step->getStringAttribute("voices", "0");
            for (int voice = 0; voice < std::min(voicesStr.length(), numVoices); voice++) {
                *(stepState[_stepIndex].voiceParameters[static_cast<size_t>(voice)]) = voicesStr[voice] == '1';
            }
            *(stepState[_stepIndex].octaveParameter) = -(step->getIntAttribute("octave", 0)) + static_cast<int>(constants::MAX_OCTAVES); // index
            *(stepState[_stepIndex].lengthParameter) = static_cast<float>(step->getDoubleAttribute("length", 0.0));
            *(stepState[_stepIndex].tieParameter) = step->getBoolAttribute("tie", false);
            *(stepState[_stepIndex].volParameter) = static_cast<float>(step->getDoubleAttribute("vol", 0.0));
            *(stepState[_stepIndex].powerParameter) = step->getBoolAttribute("power", true);
            _stepIndex++;
        }
    }

public:
    juce::AudioProcessorValueTreeState &parameters;

    juce::AudioParameterChoice *stepsParameter = nullptr;
    juce::AudioParameterChoice *voicesParameter = nullptr;
    juce::AudioParameterChoice *rateParameter = nullptr;
    juce::AudioParameterChoice *rateTypeParameter = nullptr;
    juce::AudioParameterChoice *notesParameter = nullptr;
    juce::AudioParameterChoice *modeParameter = nullptr;
    juce::AudioParameterChoice *voiceMatchingParameter = nullptr;
    std::array<StepState, constants::MAX_STEPS> stepState;

    std::atomic<stepnum_t> stepIndex{0};
    std::atomic<bool> playing{false};
    std::atomic<bool> recordButton{false};
    std::atomic<bool> recordedRest{false};

    moodycamel::ReaderWriterQueue<UpdatedSteps> updateStepsFromAudioThread{16};
};
