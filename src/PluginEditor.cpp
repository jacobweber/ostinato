#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor &p, juce::AudioProcessorValueTreeState &vts)
    : AudioProcessorEditor(&p), valueTreeState(vts)
{
    setSize(200, 200);

    speedSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    speedSlider.setRange(0.0, 1.0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    speedSlider.setPopupDisplayEnabled(true, false, this);
    speedSlider.setTextValueSuffix(" Speed");

    addAndMakeVisible(speedSlider);
    speedAttachment.reset(new SliderAttachment(valueTreeState, "speed", speedSlider));
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Speed", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    speedSlider.setBounds(40, 30, 20, getHeight() - 60);
}
