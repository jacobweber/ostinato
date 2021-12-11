#include "PluginProcessor.h"
#include "PluginEditor.h"

const size_t NUM_STEPS = 3;

PluginEditor::PluginEditor(PluginProcessor &p, juce::AudioProcessorValueTreeState &vts)
    : AudioProcessorEditor(&p), valueTreeState(vts)
{
    setSize(600, 600);

    for (size_t i = 0; i < NUM_STEPS; i++) {
        strips[i].stepNum = i;
        addAndMakeVisible(strips[i]);
    }

    speedSlider.setSliderStyle(juce::Slider::LinearBar);
    speedSlider.setRange(0.0, 1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    speedSlider.setPopupDisplayEnabled(true, false, this);
    speedSlider.setTextValueSuffix(" Speed");
    addAndMakeVisible(speedSlider);
    speedAttachment.reset(new SliderAttachment(valueTreeState, "speed", speedSlider));

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

    startTimerHz(30);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    auto area = getLocalBounds();

    timecodeDisplayLabel.setBounds(area.removeFromTop(26));
    speedSlider.setBounds(area.removeFromTop(50).reduced(8));
    messagesBox.setBounds(area.removeFromBottom(100).reduced(8));

    juce::FlexBox fb;
    for (size_t i = 0; i < NUM_STEPS; i++)
        fb.items.add(juce::FlexItem(strips[i]).withHeight((float) area.getHeight()).withWidth(100));
    fb.performLayout(area.toFloat());
}

void PluginEditor::logMessage(const juce::String &m)
{
    messagesBox.moveCaretToEnd();
    messagesBox.insertTextAtCaret(m + juce::newLine);
}

void PluginEditor::timerCallback()
{
    updateTimecodeDisplay(getProcessor().lastPosInfo.get());
}

PluginProcessor &PluginEditor::getProcessor() const
{
    return static_cast<PluginProcessor &>(processor);
}

juce::String PluginEditor::timeToTimecodeString(double seconds)
{
    auto millisecs = juce::roundToInt(seconds * 1000.0);
    auto absMillisecs = std::abs(millisecs);

    return juce::String::formatted("%02d:%02d:%02d.%03d",
                                   millisecs / 3600000,
                                   (absMillisecs / 60000) % 60,
                                   (absMillisecs / 1000) % 60,
                                   absMillisecs % 1000);
}

juce::String PluginEditor::quarterNotePositionToBarsBeatsString(double quarterNotes, int numerator, int denominator)
{
    if (numerator == 0 || denominator == 0)
        return "1|1|000";

    auto quarterNotesPerBar = (numerator * 4 / denominator);
    auto beats = (fmod(quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

    auto bar = ((int)quarterNotes) / quarterNotesPerBar + 1;
    auto beat = ((int)beats) + 1;
    auto ticks = ((int)(fmod(beats, 1.0) * 960.0 + 0.5));

    return juce::String::formatted("%d|%d|%03d", bar, beat, ticks);
}

void PluginEditor::updateTimecodeDisplay(juce::AudioPlayHead::CurrentPositionInfo pos)
{
    // ppqPosition = 0-based beat number since start; in 6/8 counts 3 beats per bar
    // ppqPositionOfLastBarStart = 0-based beat number of bar start; can be fractional e.g. in 7/8 bar 2 is 3.5

    juce::MemoryOutputStream displayText;

    displayText << juce::String(pos.bpm, 2) << " bpm, "
                << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                << "  -  " << timeToTimecodeString(pos.timeInSeconds)
                << "  -  " << quarterNotePositionToBarsBeatsString(pos.ppqPosition, pos.timeSigNumerator, pos.timeSigDenominator);

    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";

    timecodeDisplayLabel.setText(displayText.toString(), juce::dontSendNotification);
}
