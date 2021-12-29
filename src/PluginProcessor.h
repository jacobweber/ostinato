#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "MidiProcessor.h"
#include "SpinLockedPosInfo.h"
#include "State.h"
#include "Components/StepStrip.h"
#include "Props.h"

class PluginProcessor : public juce::AudioProcessor {
public:
    PluginProcessor();

    ~PluginProcessor() override;

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

    juce::AudioPlayHead::CurrentPositionInfo updateCurrentTimeInfoFromHost();

public:
    SpinLockedPosInfo lastPosInfo;

private:
    juce::AudioProcessorValueTreeState parameters;
    State state{parameters};
    MidiProcessor midiProcessor{state};
    juce::MidiBuffer midiOut;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
