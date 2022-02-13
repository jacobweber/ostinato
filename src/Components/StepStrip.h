#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include <vector>

#include "../State.h"
#include "../Constants.h"
#include "ActiveLight.h"

class StepStrip : public juce::Component {
public:
    constexpr static float ICON_SIZE = 14.0f;

    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    StepStrip(State &_state, int _stepNum);

    ~StepStrip() override;

    void paint(juce::Graphics &) override;

    void paintOverChildren(juce::Graphics &g) override;

    void resized() override;

    void refreshActiveLight();

    void refresh();

    void refreshVoices();

    void refreshActiveVoices();

    int getVoiceForPoint(int x, int y);

    void hoverVoice(int voiceNum, bool over);

private:
    int stepNum{0};
    State &state;

    juce::Font textFont{12.0f};

    ActiveLight activeLight{state, stepNum};
    juce::ImageButton clearButton{};
    juce::ImageButton fillButton{};
    juce::Label octaveLabel{{}, constants::LABEL_OCTAVE};
    juce::ComboBox octaveMenu;
    juce::Label lengthLabel{{}, constants::LABEL_LENGTH};
    juce::Slider lengthSlider;
    juce::TextButton tieButton{};
    std::vector<std::unique_ptr<juce::TextButton>> voices;
    juce::Slider volSlider;
    juce::ImageButton powerButton{};

    std::vector<std::unique_ptr<ButtonAttachment>> voicesAttachments;
    [[maybe_unused]] std::unique_ptr<ComboBoxAttachment> octaveAttachment;
    [[maybe_unused]] std::unique_ptr<SliderAttachment> lengthAttachment;
    [[maybe_unused]] std::unique_ptr<ButtonAttachment> tieAttachment;
    [[maybe_unused]] std::unique_ptr<SliderAttachment> volAttachment;
    [[maybe_unused]] std::unique_ptr<ButtonAttachment> powerAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepStrip)
};
