#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "PluginProcessor.h"

class PluginEditor : public juce::AudioProcessorEditor
{
public:
  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

  PluginEditor(PluginProcessor &, juce::AudioProcessorValueTreeState &);
  ~PluginEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState& valueTreeState;

  juce::Slider speedSlider;
  std::unique_ptr<SliderAttachment> speedAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
