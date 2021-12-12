#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Timecode.h"

PluginEditor::PluginEditor(PluginProcessor &p, State &s)
        : AudioProcessorEditor(&p), state(s) {
    for (size_t i = 0; i < NUM_STEPS; i++) {
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

    juce::FlexBox fb;
    for (size_t i = 0; i < NUM_STEPS; i++)
        if (strips.size() > i)
            fb.items.add(juce::FlexItem(*strips[i]).withHeight((float) area.getHeight()).withWidth(100));
    fb.performLayout(area.toFloat());
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
