#include "Header.h"
#include "../Scales.h"

Header::Header(State &s, PluginProcessor &p) : state(s), pluginProcessor(p) {
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
    for (size_t i = 1; i <= constants::MAX_STEPS; i++) {
        stepsMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    stepsMenu.onChange = [this] { onUpdateSteps(); };
    addAndMakeVisible(stepsMenu);
    stepsAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "steps", stepsMenu);

    addAndMakeVisible(voicesLabel);
    voicesLabel.setFont(textFont);
    voicesLabel.attachToComponent(&voicesMenu, false);
    for (size_t i = 1; i <= constants::MAX_VOICES; i++) {
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
        Scales scales{};
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
    toolbar.items.add(juce::FlexItem(stepsMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(90).withMargin(margin));
    toolbar.items.add(juce::FlexItem(voicesMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(90).withMargin(margin));
    toolbar.items.add(juce::FlexItem(rateMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130).withMargin(margin));
    toolbar.items.add(juce::FlexItem(rateTypeMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130).withMargin(margin));
    toolbar.items.add(juce::FlexItem(notesMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130).withMargin(margin));
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
