#include <vector>

#include "PluginProcessor.h"
#include "Components/PluginEditor.h"
#include "Components/StepStrip.h"
#include "Constants.h"

PluginProcessor::PluginProcessor()
        : AudioProcessor(BusesProperties()
                                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
                                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          parameters(*this, nullptr, juce::Identifier("Ostinato"), createParameterLayout()) {
}

PluginProcessor::~PluginProcessor() {
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("speed", "Arpeggiator Speed", 0.0f, 1.0f, 0.5f));
    for (size_t i = 0; i < NUM_STEPS; i++)
        for (size_t j = 0; j < NUM_VOICES; j++)
            layout.add(
                    std::make_unique<juce::AudioParameterBool>("voice_" + std::to_string(i) + "_" + std::to_string(j),
                                                               "Voice On", false));
    return layout;
}

const juce::String PluginProcessor::getName() const {
    return "Ostinato";
}

bool PluginProcessor::acceptsMidi() const {
    return true;
}

bool PluginProcessor::producesMidi() const {
    return true;
}

bool PluginProcessor::isMidiEffect() const {
    return true;
}

double PluginProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PluginProcessor::getNumPrograms() {
    return 1;
}

int PluginProcessor::getCurrentProgram() {
    return 0;
}

void PluginProcessor::setCurrentProgram(int) {
}

const juce::String PluginProcessor::getProgramName(int) {
    return {};
}

void PluginProcessor::changeProgramName(int, const juce::String &) {
}

void PluginProcessor::prepareToPlay(double sampleRate, int) {
    midiProcessor.init(sampleRate);
}

void PluginProcessor::releaseResources() {
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    juce::ignoreUnused(layouts);
    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    buffer.clear();

    const auto posInfo = updateCurrentTimeInfoFromHost();
    lastPosInfo.set(posInfo);

    auto numSamples = buffer.getNumSamples();
    //speedParameter->get()
    midiProcessor.process(numSamples, midiMessages, posInfo, state);
}

bool PluginProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor *PluginProcessor::createEditor() {
    return new PluginEditor(*this, state);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData) {
    auto parametersCopy = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(parametersCopy.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioPlayHead::CurrentPositionInfo PluginProcessor::updateCurrentTimeInfoFromHost() {
    if (auto *ph = getPlayHead()) {
        juce::AudioPlayHead::CurrentPositionInfo result;

        if (ph->getCurrentPosition(result))
            return result;
    }

    // If the host fails to provide the current time, we'll just use default values
    juce::AudioPlayHead::CurrentPositionInfo result;
    result.resetToDefault();
    return result;
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new PluginProcessor();
}
