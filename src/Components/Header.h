#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "../PluginProcessor.h"
#include "../State.h"
#include "../Scales.h"
#include "../Constants.h"
#include "FileButton.h"

class Header : public juce::Component {
public:
    constexpr static float ICON_SIZE = 24.0f;
    constexpr static float MENU_HEIGHT = 24.0f;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    Header(State &s, PluginProcessor &p);

    void showSettingsMenu();

    static void settingsMenuItemChosenCallback(int result, Header* component);

    void timerCallback();

    void refresh();

    void refreshMessage();

    void refreshEnabled();

    void paint(juce::Graphics &) override;

    void resized() override;

public:
    std::function<void()> onUpdateSteps = [] {};
    std::function<void()> onUpdateVoices = [] {};
    std::function<void()> onClickRecord = [] {};
    std::function<void()> onClickRandom = [] {};

private:
    State &state;
    PluginProcessor &pluginProcessor;
    Scales scales{};

    juce::Font textFont{12.0f};
    juce::Font messageFont{16.0f};

    FileButton fileButton{state};
    juce::ImageButton recordButton{};
    juce::ImageButton randomButton{};
    juce::Label stepsLabel{{}, constants::LABEL_STEPS};
    juce::ComboBox stepsMenu;
    juce::Label voicesLabel{{}, constants::LABEL_VOICES};
    juce::ComboBox voicesMenu;
    juce::Label rateLabel{{}, constants::LABEL_RATE};
    juce::ComboBox rateMenu;
    juce::ComboBox rateTypeMenu;
    juce::Label modeLabel{{}, constants::LABEL_MODE};
    juce::ComboBox modeMenu;
    juce::Label scaleLabel{{}, constants::LABEL_SCALE};
    juce::ComboBox scaleMenu;
    juce::Label chordScaleLabel{{}, constants::LABEL_CHORD_SCALE};
    juce::ComboBox chordScaleMenu;
    juce::Label keyLabel{{}, constants::LABEL_KEY};
    juce::ComboBox keyMenu;
    juce::ImageButton settingsButton{};
    juce::Label messageLabel{{}};

    // use unique_ptr so it can be destroyed when UI is
    // declare after components so it's destroyed before them
    std::unique_ptr<ComboBoxAttachment> stepsAttachment;
    std::unique_ptr<ComboBoxAttachment> voicesAttachment;
    std::unique_ptr<ComboBoxAttachment> rateAttachment;
    std::unique_ptr<ComboBoxAttachment> rateTypeAttachment;
    std::unique_ptr<ComboBoxAttachment> modeAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleAttachment;
    std::unique_ptr<ComboBoxAttachment> chordScaleAttachment;
    std::unique_ptr<ComboBoxAttachment> keyAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Header)
};
