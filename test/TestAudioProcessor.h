#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class TestAudioProcessor : public juce::AudioProcessor {
public:
    TestAudioProcessor() = default;

    explicit TestAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
            : state(*this, nullptr, "state", std::move(layout)) {}

    const juce::String getName() const override { return {}; }

    void prepareToPlay(double, int) override {}

    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override {}

    using juce::AudioProcessor::processBlock;

    double getTailLengthSeconds() const override { return {}; }

    bool acceptsMidi() const override { return {}; }

    bool producesMidi() const override { return {}; }

    juce::AudioProcessorEditor *createEditor() override { return {}; }

    bool hasEditor() const override { return {}; }

    int getNumPrograms() override { return 1; }

    int getCurrentProgram() override { return {}; }

    void setCurrentProgram(int) override {}

    const juce::String getProgramName(int) override { return {}; }

    void changeProgramName(int, const juce::String &) override {}

    void getStateInformation(juce::MemoryBlock &) override {}

    void setStateInformation(const void *, int) override {}

    juce::AudioProcessorValueTreeState state{*this, nullptr};
};
