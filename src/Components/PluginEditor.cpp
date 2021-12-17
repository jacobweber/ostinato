#include "../PluginProcessor.h"
#include "PluginEditor.h"
#include "../Timecode.h"
#include "../Constants.h"

PluginEditor::PluginEditor(PluginProcessor &p, State &s)
        : AudioProcessorEditor(&p), state(s) {
    for (size_t i = 0; i < MAX_STEPS; i++) {
        strips.push_back(std::unique_ptr<StepStrip>(new StepStrip(state, i)));
        addAndMakeVisible(*strips[i]);
    }

    speedSlider.setSliderStyle(juce::Slider::LinearBar);
    speedSlider.setRange(0.0, 1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    speedSlider.setPopupDisplayEnabled(true, false, this);
    speedSlider.setTextValueSuffix(" Speed");
    addAndMakeVisible(speedSlider);
    speedAttachment.reset(new SliderAttachment(state.parameters, "speed", speedSlider));

    addAndMakeVisible(stepsLabel);
    stepsLabel.setFont(textFont);
    stepsLabel.attachToComponent(&stepsMenu, false);
    for (size_t i = 1; i <= MAX_STEPS; i++) {
        stepsMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    addAndMakeVisible(stepsMenu);
    stepsAttachment.reset(new ComboBoxAttachment(state.parameters, "steps", stepsMenu));

    addAndMakeVisible(voicesLabel);
    voicesLabel.setFont(textFont);
    voicesLabel.attachToComponent(&voicesMenu, false);
    for (size_t i = 1; i <= MAX_VOICES; i++) {
        voicesMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    addAndMakeVisible(voicesMenu);
    voicesAttachment.reset(new ComboBoxAttachment(state.parameters, "voices", voicesMenu));

    addAndMakeVisible(rateLabel);
    rateLabel.setFont(textFont);
    rateLabel.attachToComponent(&rateMenu, false);
    rateMenu.addItem("Whole", 1);
    rateMenu.addItem("Half", 2);
    rateMenu.addItem("Quarter", 4);
    rateMenu.addItem("Eighth", 8);
    rateMenu.addItem("Sixteenth", 16);
    addAndMakeVisible(rateMenu);
    rateAttachment.reset(new ComboBoxAttachment(state.parameters, "rate", rateMenu));

    addAndMakeVisible(messagesBox);
    messagesBox.setMultiLine(true);
    messagesBox.setReturnKeyStartsNewLine(true);
    messagesBox.setReadOnly(true);
    messagesBox.setScrollbarsShown(true);
    messagesBox.setCaretVisible(false);
    messagesBox.setPopupMenuEnabled(true);
    messagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    messagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    messagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    addAndMakeVisible(timecodeDisplayLabel);
    timecodeDisplayLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 15.0f, juce::Font::plain));

    setSize(600, 600); // resize after initialization
    startTimerHz(30);
}

PluginEditor::~PluginEditor() {
}

void PluginEditor::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized() {
    auto area = getLocalBounds();

    timecodeDisplayLabel.setBounds(area.removeFromTop(26));
    speedSlider.setBounds(area.removeFromTop(50).reduced(8));
    messagesBox.setBounds(area.removeFromBottom(100).reduced(8));

    juce::FlexBox top;
    juce::Rectangle<int> topArea = area.removeFromTop(40).reduced(8);
    top.items.add(juce::FlexItem(stepsMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(voicesMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(rateMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.performLayout(topArea.toFloat());

    juce::FlexBox channelStrip;
    for (size_t i = 0; i < MAX_STEPS; i++)
        if (strips.size() > i)
            channelStrip.items.add(juce::FlexItem(*strips[i]).withHeight((float) area.getHeight()).withWidth(100));
    channelStrip.performLayout(area.toFloat());
}

void PluginEditor::logMessage(const juce::String &m) {
    messagesBox.moveCaretToEnd();
    messagesBox.insertTextAtCaret(m + juce::newLine);
}

void PluginEditor::timerCallback() {
    juce::String newText = updateTimecodeDisplay(getProcessor().lastPosInfo.get());
    timecodeDisplayLabel.setText(newText, juce::dontSendNotification);
}

PluginProcessor &PluginEditor::getProcessor() const {
    return static_cast<PluginProcessor &>(processor);
}
