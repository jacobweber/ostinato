#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "PluginProcessor.h"

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

  PluginEditor(PluginProcessor &, juce::AudioProcessorValueTreeState &);
  ~PluginEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  void logMessage(const juce::String &);
  void timerCallback() override;
  PluginProcessor &getProcessor() const;

  static juce::String timeToTimecodeString(double seconds);
  static juce::String quarterNotePositionToBarsBeatsString(double quarterNotes, int numerator, int denominator);
  void updateTimecodeDisplay(juce::AudioPlayHead::CurrentPositionInfo pos);

private:
  juce::AudioProcessorValueTreeState &valueTreeState;

  juce::Label timecodeDisplayLabel;

  juce::Slider speedSlider;
  std::unique_ptr<SliderAttachment> speedAttachment;

  juce::TextEditor messagesBox;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
