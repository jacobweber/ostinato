#include "Header.h"

Header::Header(State &s, PluginProcessor &p) : state(s), pluginProcessor(p) {
    juce::Image floppyDisk = FontAwesome::getInstance()->getIcon(true,
                                                           juce::String::fromUTF8(
                                                                   reinterpret_cast<const char *>(u8"\uf0c7")),
                                                           ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                           1);
    fileButton.setImages(true, false, true, floppyDisk, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, constants::COLOR_TOGGLE_INACTIVE);
    fileButton.setTooltip(constants::TOOLTIP_RANDOM);
    fileButton.onClick = [&] {
        juce::PopupMenu menu;
        menu.addItem(1, "Reset to Defaults");
        menu.addItem(2, "Open Presets Folder");
        menu.addItem(3, juce::CharPointer_UTF8("Save Preset\u2026"));
        menu.addItem(4, juce::CharPointer_UTF8("Export Preset\u2026"));
        menu.addItem(5, juce::CharPointer_UTF8("Import Preset\u2026"));
        menu.showMenuAsync(juce::PopupMenu::Options{}.withTargetComponent(fileButton),
            juce::ModalCallbackFunction::forComponent(fileMenuItemChosenCallback, this));
    };
    addAndMakeVisible(fileButton);

    juce::Image microphone = FontAwesome::getInstance()->getIcon(true,
                                                                 juce::String::fromUTF8(
                                                                         reinterpret_cast<const char *>(u8"\uf130")),
                                                                 ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                                 1);
    recordButton.setImages(true, false, true, microphone, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                           constants::COLOR_TOGGLE_INACTIVE);
    recordButton.setClickingTogglesState(true);
    recordButton.setTooltip(constants::TOOLTIP_RECORD);
    addAndMakeVisible(recordButton);
    recordButton.onClick = [this] {
        state.recordButton = recordButton.getToggleState();
        refreshMessage();
        refreshEnabled();
        onClickRecord();
    };

    juce::Image arrowsLeftRight = FontAwesome::getInstance()->getIcon(true,
                                                                      juce::String::fromUTF8(
                                                                              reinterpret_cast<const char *>(u8"\uf07e")),
                                                                      ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                                      1);
    stretchButton.setImages(true, false, true, arrowsLeftRight, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                            constants::COLOR_TOGGLE_INACTIVE);
    stretchButton.setClickingTogglesState(true);
    stretchButton.setTooltip(constants::TOOLTIP_STRETCH);
    addAndMakeVisible(stretchButton);
    stretchButton.onClick = [this] {
        refreshMessage();
    };
    stretchAttachment = std::make_unique<ButtonAttachment>(state.parameters, "stretch", stretchButton);

    juce::Image dice = FontAwesome::getInstance()->getIcon(true,
                                                           juce::String::fromUTF8(
                                                                   reinterpret_cast<const char *>(u8"\uf522")),
                                                           ICON_SIZE, constants::COLOR_TOGGLE_ACTIVE,
                                                           1);
    randomButton.setImages(true, false, true, dice, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, constants::COLOR_TOGGLE_INACTIVE);
    randomButton.setTooltip(constants::TOOLTIP_RANDOM);
    randomButton.onClick = [this] { onClickRandom(); };
    addAndMakeVisible(randomButton);

    addAndMakeVisible(stepsLabel);
    stepsLabel.setFont(textFont);
    stepsLabel.attachToComponent(&stepsMenu, false);
    for (stepnum_t i = 1; i <= constants::MAX_STEPS; i++) {
        stepsMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    stepsMenu.onChange = [this] { onUpdateSteps(); };
    addAndMakeVisible(stepsMenu);
    stepsAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "steps", stepsMenu);

    addAndMakeVisible(voicesLabel);
    voicesLabel.setFont(textFont);
    voicesLabel.attachToComponent(&voicesMenu, false);
    for (voicenum_t i = 1; i <= constants::MAX_VOICES; i++) {
        voicesMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    voicesMenu.onChange = [this] { onUpdateVoices(); };
    addAndMakeVisible(voicesMenu);
    voicesAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "voices", voicesMenu);

    addAndMakeVisible(rateLabel);
    rateLabel.setFont(textFont);
    rateLabel.attachToComponent(&rateMenu, false);
    int rateIndex = 1;
    for (const juce::String &value: state.rateParameter->getAllValueStrings()) {
        rateMenu.addItem(value, rateIndex++);
    }
    addAndMakeVisible(rateMenu);
    rateAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "rate", rateMenu);

    int rateTypeIndex = 1;
    for (const juce::String &value: state.rateTypeParameter->getAllValueStrings()) {
        rateTypeMenu.addItem(value, rateTypeIndex++);
    }
    addAndMakeVisible(rateTypeMenu);
    rateTypeAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "rateType", rateTypeMenu);

    addAndMakeVisible(notesLabel);
    notesLabel.setFont(textFont);
    notesLabel.attachToComponent(&notesMenu, false);
    int notesIndex = 1;
    for (const juce::String &value: state.notesParameter->getAllValueStrings()) {
        notesMenu.addItem(value, notesIndex++);
    }
    addAndMakeVisible(notesMenu);
    notesAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "notes", notesMenu);
    notesMenu.onChange = [this] {
        // shouldn't do this in UI
        int notesSource = state.notesParameter->getIndex();
        if (notesSource != 0) {
            const std::vector<int> &scale = scales.allScales[static_cast<size_t>(notesSource) - 1];
            *(state.voicesParameter) = static_cast<int>(juce::jmax(constants::MAX_VOICES, scale.size() + 1));
            *(state.stretchParameter) = false;
        }
        refreshMessage();
        refreshEnabled();
    };

    addAndMakeVisible(messageLabel);
    messageLabel.setFont(messageFont);
    messageLabel.setColour(juce::Label::ColourIds::textColourId, constants::COLOR_MESSAGE_TEXT);

    refresh();
}

void Header::fileMenuItemChosenCallback(int result, Header* component) {
    if (component == nullptr) return;
    switch (result) {
        case 1: // reset
            component->state.resetToDefaults();
            break;
        case 2: // show
            component->showPresetsDir();
            break;
        case 3: // save
            component->showSaveDialog();
            break;
        case 4: // export
            component->showExportDialog();
            break;
        case 5: // import
            component->showImportDialog();
            break;
    }
}

void Header::saveDialogClosedCallback(int result, Header* component) {
    if (component == nullptr) return;
    auto& aw = *(component->asyncAlertWindow);
    aw.exitModalState(result);
    aw.setVisible(false);
    if (result != 1) return;
    auto presetName = aw.getTextEditorContents("presetName");
    auto fileName = juce::File::createLegalFileName(presetName + ".xml");
    auto presetsDir = component->getPresetsDir();
    if (!presetsDir.exists() && !presetsDir.createDirectory().wasOk()) return;
    component->state.saveToFile(presetsDir.getChildFile(fileName));
}

void Header::showPresetsDir() {
    auto presetsDir = getPresetsDir();
    if (!presetsDir.exists() && !presetsDir.createDirectory().wasOk()) return;
    presetsDir.revealToUser();
}

juce::File Header::getPresetsDir() {
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Application Support")
        .getChildFile(juce::JUCEApplication::getInstance()->getApplicationName())
        .getChildFile("Presets");
}

void Header::showSaveDialog() {
    asyncAlertWindow = std::make_unique<juce::AlertWindow>("Save Preset",
        "Enter the preset name.", juce::MessageBoxIconType::NoIcon);
    asyncAlertWindow->addTextEditor("presetName", "", "Name:");
    asyncAlertWindow->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    asyncAlertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));
    asyncAlertWindow->getTextEditor("presetName")->setExplicitFocusOrder(1);
    asyncAlertWindow->enterModalState(true, juce::ModalCallbackFunction::forComponent(saveDialogClosedCallback, this));
}

void Header::showExportDialog() {
    fc.reset(new juce::FileChooser("Select filename to export presets to.", {}, "*.xml",  true));
    fc->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& chooser) {
            auto result = chooser.getURLResult();
            if (result.isEmpty()) return;
            if (!result.isLocalFile()) return;
            state.saveToFile(result.getLocalFile());
        }
    );
}

void Header::showImportDialog() {
    fc.reset(new juce::FileChooser("Select presets file to import.", {}, "*.xml", true));
    fc->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& chooser) {
            juce::String chosen;
            auto result = chooser.getURLResult();
            if (result.isEmpty()) return;
            if (!result.isLocalFile()) return;
            state.loadFromFile(result.getLocalFile());
        }
    );
}

void Header::timerCallback() {
    if (state.recordButton != recordButton.getToggleState()) {
        recordButton.setToggleState(state.recordButton, juce::NotificationType::sendNotification);
    }
}

void Header::refresh() {
    refreshMessage();
    refreshEnabled();
}

void Header::refreshMessage() {
    juce::String text;
    if (recordButton.getToggleState()) {
        text = constants::MSG_RECORD;
    } else if (stretchButton.getToggleState()) {
        text = constants::MSG_STRETCH;
    } else if (notesMenu.getSelectedItemIndex() > 0) {
        text = constants::MSG_SCALE;
    } else {
        text = "";
    }
    messageLabel.setText(text, juce::NotificationType::dontSendNotification);
    repaint();
}

void Header::refreshEnabled() {
    const bool notRecording = !state.recordButton;
    fileButton.setEnabled(notRecording);
    stepsMenu.setEnabled(notRecording);
    voicesMenu.setEnabled(notRecording);
    rateMenu.setEnabled(notRecording);
    rateTypeMenu.setEnabled(notRecording);
    notesMenu.setEnabled(notRecording);
    stretchButton.setEnabled(notRecording);
    int notesSource = state.notesParameter->getIndex();
    stretchButton.setEnabled(notRecording && notesSource == 0);
    randomButton.setEnabled(notRecording);
}

void Header::paint(juce::Graphics &g) {
    auto area = getLocalBounds();
    if (messageLabel.getText() == "") {
        area = area.withTrimmedBottom(25);
    }
    g.setColour(constants::COLOR_HEADER);
    g.fillRect(area);
}

void Header::resized() {
    auto area = getLocalBounds();
    messageLabel.setBounds(area.removeFromBottom(25).withTrimmedRight(4).withTrimmedLeft(4).withTrimmedBottom(4));

    auto toolbarRect = area.removeFromBottom(48).reduced(8);
    juce::FlexBox toolbar;
    juce::FlexItem::Margin margin{0.0, 5.0, 0.0, 0.0};
    toolbar.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    toolbar.alignItems = juce::FlexBox::AlignItems::center;
    toolbar.items.add(
            juce::FlexItem(fileButton).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
                    ICON_SIZE + 10).withWidth(
                    static_cast<float>(fileButton.getWidth())).withMargin(margin));
    toolbar.items.add(juce::FlexItem(stepsMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(90.0).withMargin(margin));
    toolbar.items.add(juce::FlexItem(voicesMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(90.0).withMargin(margin));
    toolbar.items.add(juce::FlexItem(rateMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130.0).withMargin(margin));
    toolbar.items.add(juce::FlexItem(rateTypeMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130.0).withMargin(margin));
    toolbar.items.add(juce::FlexItem(notesMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130.0).withMargin(margin));
    toolbar.items.add(juce::FlexItem(recordButton).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            ICON_SIZE + 10).withWidth(
            static_cast<float>(recordButton.getWidth())).withMargin({0.0, 5.0, 0.0, 5.0}));
    toolbar.items.add(juce::FlexItem(stretchButton).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            ICON_SIZE + 10).withWidth(
            static_cast<float>(stretchButton.getWidth())).withMargin({0.0, 5.0, 0.0, 5.0}));
    toolbar.items.add(
            juce::FlexItem(randomButton).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
                    ICON_SIZE + 10).withWidth(
                    static_cast<float>(randomButton.getWidth())).withMargin({0.0, 0.0, 0.0, 5.0}));
    toolbar.performLayout(toolbarRect);
}
