#include "../PluginProcessor.h"
#include "PluginEditor.h"
#include "../Timecode.h"
#include "../Constants.h"

PluginEditor::PluginEditor(PluginProcessor &p, State &s)
        : AudioProcessorEditor(&p), state(s) {
    addAndMakeVisible(timecodeDisplayLabel);
    timecodeDisplayLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 15.0f, juce::Font::plain));

    addAndMakeVisible(stepsLabel);
    stepsLabel.setFont(textFont);
    stepsLabel.attachToComponent(&stepsMenu, false);
    for (size_t i = 1; i <= MAX_STEPS; i++) {
        stepsMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    stepsMenu.onChange = [this] {
        refreshSize();
        channelStrips.refresh();
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
        channelStrips.refresh();
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

    addAndMakeVisible(channelStrips);

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

    refreshSize(); // resize after initialization
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
    messagesBox.setBounds(area.removeFromBottom(100).reduced(8));

    juce::FlexBox top;
    juce::Rectangle<int> topArea = area.removeFromTop(40).reduced(8);
    top.items.add(juce::FlexItem(stepsMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(voicesMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.items.add(juce::FlexItem(rateMenu).withHeight((float) topArea.getHeight()).withWidth(150));
    top.performLayout(topArea.toFloat());

    channelStrips.setBounds(area);
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

void PluginEditor::refreshSize() {
    int numSteps = state.stepsParameter->getIndex() + 1;
    int width = juce::jmin(1200, juce::jmax(600, 100 * numSteps));
    setSize(width, 600);
}