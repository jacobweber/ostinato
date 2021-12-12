#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "MidiProcessor.h"
#include "SpinLockedPosInfo.h"
#include "State.h"

class PluginProcessor : public juce::AudioProcessor {
public:
    PluginProcessor();

    ~PluginProcessor() override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    void updateCurrentTimeInfoFromHost();

public:
    SpinLockedPosInfo lastPosInfo;

private:
    std::atomic<float> *speedParameter = nullptr;
    juce::AudioProcessorValueTreeState parameters;
    State state{parameters};
    MidiProcessor midiProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
