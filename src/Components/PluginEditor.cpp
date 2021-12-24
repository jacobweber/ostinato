#include <random>

#include "../PluginProcessor.h"
#include "PluginEditor.h"
#include "../Timecode.h"
#include "../Constants.h"

PluginEditor::PluginEditor(PluginProcessor &p, State &s)
        : AudioProcessorEditor(&p), state(s) {
    addAndMakeVisible(timecodeDisplayLabel);
    timecodeDisplayLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 15.0f, juce::Font::plain));

    juce::Image dice = FontAwesome::getInstance()->getIcon(true,
                                                           juce::String::fromUTF8(
                                                                   reinterpret_cast<const char *>(u8"\uf522")),
                                                           ICON_SIZE, juce::Colours::red,
                                                           1);
    randomButton.setImages(true, false, true, dice, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, juce::Colours::white);
    randomButton.onClick = [this] { randomizeParams(true); };
    addAndMakeVisible(randomButton);

    addAndMakeVisible(stepsLabel);
    stepsLabel.setFont(textFont);
    stepsLabel.attachToComponent(&stepsMenu, false);
    for (size_t i = 1; i <= MAX_STEPS; i++) {
        stepsMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    stepsMenu.onChange = [this] {
        refreshSize();
        stepStrips.refresh();
    };
    addAndMakeVisible(stepsMenu);
    stepsAttachment.reset(new ComboBoxAttachment(state.parameters, "steps", stepsMenu));

    addAndMakeVisible(voicesLabel);
    voicesLabel.setFont(textFont);
    voicesLabel.attachToComponent(&voicesMenu, false);
    for (size_t i = 1; i <= MAX_VOICES; i++) {
        voicesMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    voicesMenu.onChange = [this] {
        refreshSize();
        stepStrips.refresh();
    };
    addAndMakeVisible(voicesMenu);
    voicesAttachment.reset(new ComboBoxAttachment(state.parameters, "voices", voicesMenu));

    addAndMakeVisible(rateLabel);
    rateLabel.setFont(textFont);
    rateLabel.attachToComponent(&rateMenu, false);
    rateMenu.addItem("Whole", 1);
    rateMenu.addItem("Half", 2);
    rateMenu.addItem("Quarter", 3);
    rateMenu.addItem("Eighth", 4);
    rateMenu.addItem("Sixteenth", 5);
    rateMenu.addItem("Thirty-Second", 6);
    rateMenu.addItem("Sixty-Fourth", 7);
    addAndMakeVisible(rateMenu);
    rateAttachment.reset(new ComboBoxAttachment(state.parameters, "rate", rateMenu));

    rateTypeMenu.addItem("Straight", 1);
    rateTypeMenu.addItem("Triplet", 2);
    rateTypeMenu.addItem("Dotted", 3);
    addAndMakeVisible(rateTypeMenu);
    rateTypeAttachment.reset(new ComboBoxAttachment(state.parameters, "rateType", rateTypeMenu));

    addAndMakeVisible(stepStrips);

    refreshSize(); // resize after initialization
    startTimerHz(60);
}

PluginEditor::~PluginEditor() {
}

void PluginEditor::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized() {
    auto area = getLocalBounds();

    juce::FlexBox headerBox;
    headerBox.justifyContent = juce::FlexBox::JustifyContent::center;
    headerBox.items.add(juce::FlexItem(timecodeDisplayLabel).withFlex(1));
    headerBox.items.add(juce::FlexItem(randomButton).withWidth(randomButton.getWidth()).withMargin(5));
    headerBox.performLayout(area.removeFromTop(26));

    juce::FlexBox top;
    juce::Rectangle<int> topArea = area.removeFromTop(40).reduced(8);
    top.items.add(juce::FlexItem(stepsMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(voicesMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(rateMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(rateTypeMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.performLayout(topArea.toFloat());

    stepStrips.setBounds(area);
}

void PluginEditor::timerCallback() {
    juce::String newText = updateTimecodeDisplay(getProcessor().lastPosInfo.get());
    timecodeDisplayLabel.setText(newText, juce::dontSendNotification);
    stepStrips.refreshActiveStep();
}

PluginProcessor &PluginEditor::getProcessor() const {
    return static_cast<PluginProcessor &>(processor);
}

void PluginEditor::refreshSize() {
    int numSteps = state.stepsParameter->getIndex() + 1;
    int width = juce::jmin(1200, juce::jmax(600, 100 * numSteps));
    setSize(width, 600);
}

void PluginEditor::randomizeParams(bool stepsAndVoices) {
    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_int_distribution<size_t> randNumSteps(1, MAX_STEPS);
    std::uniform_int_distribution<size_t> randNumVoices(1, MAX_VOICES);
    std::uniform_int_distribution<int> randRate(1, state.rateParameter->getAllValueStrings().size());
    std::uniform_int_distribution<int> randRateType(1, state.rateTypeParameter->getAllValueStrings().size());

    std::uniform_int_distribution<int> randVoice(0, 3);
    std::uniform_int_distribution<int> randOctave(1, static_cast<int>(MAX_VOICES) * 2 + 1);
    std::uniform_real_distribution<float> randLength(0.0, 1.0);
    std::uniform_int_distribution<int> randTie(0, 10);
    std::uniform_real_distribution<float> randVolume(0.0, 1.0);
    std::uniform_int_distribution<int> randPower(0, 10);

    size_t numSteps;
    size_t numVoices;
    if (stepsAndVoices) {
        numSteps = randNumSteps(mt);
        numVoices = randNumVoices(mt);
        *(state.stepsParameter) = static_cast<int>(numSteps - 1); // index
        *(state.voicesParameter) = static_cast<int>(numVoices - 1); // index
    } else {
        numSteps = static_cast<size_t>(state.stepsParameter->getIndex() + 1);
        numVoices = static_cast<size_t>(state.voicesParameter->getIndex() + 1);
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
