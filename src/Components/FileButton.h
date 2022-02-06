#pragma once

#include "juce_audio_utils/juce_audio_utils.h"

#include "../State.h"
#include "../Constants.h"

class FileButton : public juce::ImageButton {
public:
    explicit FileButton(State &_state);

    void showFileMenu();

private:
    static void fileMenuItemChosenCallback(int result, FileButton* component);

    static void saveDialogClosedCallback(int result, FileButton* component);

    void showPresetsDir();

    juce::File getPresetsDir();

    void refreshPresetNames();

    void showSaveDialog();

    void showExportDialog();

    void showImportDialog();

private:
    State &state;
    juce::String lastPresetName{""};
    juce::Array<juce::File> presetNames{};

    std::unique_ptr<juce::FileChooser> fc;
    std::unique_ptr<juce::AlertWindow> asyncAlertWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileButton)
};
