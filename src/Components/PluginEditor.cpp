#include <memory>
#include <random>

#include "../PluginProcessor.h"
#include "PluginEditor.h"
#include "../Step.h"

PluginEditor::PluginEditor(PluginProcessor &p, State &s)
        : AudioProcessorEditor(&p), state(s), pluginProcessor(p) {

    setLookAndFeel(&lookAndFeel);

    header.onUpdateSteps = [this] {
        refreshSize();
        stepStrips.refresh();
    };
    header.onUpdateVoices = [this] {
        refreshSize();
        stepStrips.refresh();
    };
    header.onClickRandom = [this] {
        randomizeParams(true);
    };
    addAndMakeVisible(header);
    addAndMakeVisible(stepStrips);

    refreshSize(); // resize after initialization
    startTimerHz(60);

    setWantsKeyboardFocus(true);
    addKeyListener(this);
}

PluginEditor::~PluginEditor() {
    removeKeyListener(this);
    setLookAndFeel(nullptr);
}

bool PluginEditor::keyPressed(const juce::KeyPress &key) {
    if (state.recordButton && key.isKeyCode(juce::KeyPress::spaceKey)) {
        state.recordedRest = true;
        return true;
    }
    return false;
}

bool PluginEditor::keyPressed(const juce::KeyPress &key, juce::Component *) {
    return keyPressed(key);
}

void PluginEditor::paint(juce::Graphics &g) {
    juce::ColourGradient gradient(props::COLOR_BACKGROUND, 0, 0, (props::COLOR_BACKGROUND).brighter(.25f), getWidth(),
                                  0, false);
    g.setGradientFill(gradient);
    g.fillAll();
}

void PluginEditor::resized() {
    auto area = getLocalBounds();
    header.setBounds(area.removeFromTop(60));
    stepStrips.setBounds(area);
}

void PluginEditor::timerCallback() {
    stepStrips.refreshActiveStep();
    header.timerCallback();
}

void PluginEditor::refreshSize() {
    int numSteps = state.stepsParameter->getIndex() + 1;
    int width = juce::jmin(1200, juce::jmax(600, 100 * numSteps));
    setSize(width, 600);
}

void PluginEditor::randomizeParams(bool stepsAndVoices) {
    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_int_distribution<size_t> randNumSteps(1, props::MAX_STEPS);
    std::uniform_int_distribution<size_t> randNumVoices(1, props::MAX_VOICES);
    std::uniform_int_distribution<int> randRate(1, state.rateParameter->getAllValueStrings().size());
    std::uniform_int_distribution<int> randRateType(1, state.rateTypeParameter->getAllValueStrings().size());

    std::uniform_int_distribution<int> randVoice(0, 3);
    std::uniform_int_distribution<int> randOctave(0, static_cast<int>(props::MAX_OCTAVES) * 2);
    std::uniform_real_distribution<float> randLength(0.0, 1.0);
    std::uniform_int_distribution<int> randTie(0, 10);
    std::uniform_real_distribution<float> randVolume(0.0, 1.0);
    std::uniform_int_distribution<int> randPower(0, 10);

    *(state.stretchParameter) = false;
    size_t numSteps;
    size_t numVoices;
    if (stepsAndVoices) {
        numSteps = randNumSteps(mt);
        numVoices = randNumVoices(mt);
        *(state.stepsParameter) = static_cast<int>(numSteps - 1); // index
        *(state.voicesParameter) = static_cast<int>(numVoices - 1); // index
    } else {
        numSteps = static_cast<size_t>(state.stepsParameter->getIndex()) + 1;
        numVoices = static_cast<size_t>(state.voicesParameter->getIndex()) + 1;
    }
    *(state.rateParameter) = randRate(mt); // index
    *(state.rateTypeParameter) = randRateType(mt); // index
    for (size_t i = 0; i < numSteps; i++) {
        for (size_t j = 0; j < numVoices; j++) {
            *(state.stepState[i].voiceParameters[j]) = randVoice(mt) == 0;
        }
        *(state.stepState[i].octaveParameter) = randOctave(mt); // index
        *(state.stepState[i].lengthParameter) = randLength(mt);
        *(state.stepState[i].tieParameter) = randTie(mt) == 0;
        *(state.stepState[i].volParameter) = randVolume(mt);
        *(state.stepState[i].powerParameter) = randPower(mt) != 0;
    }
}
