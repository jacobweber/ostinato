#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

const size_t NUM_VOICES = 4;

class StepStrip : public juce::Component
{
public:
  StepStrip()
  {
    for (size_t i = 0; i < NUM_VOICES; i++)
    {
      voices[i].setClickingTogglesState(true);
      voices[i].setButtonText(std::to_string(i + 1));
      voices[i].setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
      addAndMakeVisible(voices[i]);
    }
  }

  void paint(juce::Graphics &g) override
  {
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("#" + std::to_string(stepNum + 1), getLocalBounds().withHeight(20), juce::Justification::centred, true);

    g.setColour(juce::Colours::red);
    g.drawRect(getLocalBounds().reduced(1), 2.0f);
  }

  void resized() override
  {
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);
    for (size_t i = 0; i < NUM_VOICES; i++)
    {
      voices[i].setBounds(area.removeFromTop(20));
      area.removeFromTop(2);
    }
  }

  size_t stepNum = 0;

private:
  std::array<juce::TextButton, 4> voices;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrip)
};
