#include "Header.h"
#include "../Timecode.h"

Header::Header(State &s, PluginProcessor &p) : state(s), pluginProcessor(p) {
    addAndMakeVisible(timecodeDisplayLabel);
    timecodeDisplayLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 15.0f, juce::Font::plain));

    juce::Image arrowsLeftRight = FontAwesome::getInstance()->getIcon(true,
                                                                      juce::String::fromUTF8(
                                                                              reinterpret_cast<const char *>(u8"\uf07e")),
                                                                      ICON_SIZE, props::COLOR_TOGGLE_ACTIVE,
                                                                      1);
    stretchButton.setImages(true, false, true, arrowsLeftRight, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f,
                            props::COLOR_TOGGLE_INACTIVE);
    stretchButton.setClickingTogglesState(true);
    stretchButton.setTooltip(props::TOOLTIP_STRETCH);
    addAndMakeVisible(stretchButton);
    stretchAttachment = std::make_unique<ButtonAttachment>(state.parameters, "stretch", stretchButton);

    juce::Image dice = FontAwesome::getInstance()->getIcon(true,
                                                           juce::String::fromUTF8(
                                                                   reinterpret_cast<const char *>(u8"\uf522")),
                                                           ICON_SIZE, props::COLOR_TOGGLE_ACTIVE,
                                                           1);
    randomButton.setImages(true, false, true, dice, 1.0f, {}, {}, 1.0f, {}, {}, 1.0f, props::COLOR_TOGGLE_INACTIVE);
    randomButton.setTooltip(props::TOOLTIP_RANDOM);
    randomButton.onClick = [this] { onClickRandom(); };
    addAndMakeVisible(randomButton);

    addAndMakeVisible(stepsLabel);
    stepsLabel.setFont(textFont);
    stepsLabel.attachToComponent(&stepsMenu, false);
    for (size_t i = 1; i <= props::MAX_STEPS; i++) {
        stepsMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    stepsMenu.onChange = [this] { onUpdateSteps(); };
    addAndMakeVisible(stepsMenu);
    stepsAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "steps", stepsMenu);

    addAndMakeVisible(voicesLabel);
    voicesLabel.setFont(textFont);
    voicesLabel.attachToComponent(&voicesMenu, false);
    for (size_t i = 1; i <= props::MAX_VOICES; i++) {
        voicesMenu.addItem(std::to_string(i), static_cast<int>(i));
    }
    voicesMenu.onChange = [this] { onUpdateVoices(); };
    addAndMakeVisible(voicesMenu);
    voicesAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "voices", voicesMenu);

    addAndMakeVisible(rateLabel);
    rateLabel.setFont(textFont);
    rateLabel.attachToComponent(&rateMenu, false);
    rateMenu.addItem("Whole", 1);
    rateMenu.addItem("Half", 2);
    rateMenu.addItem("Quarter", 3);
    rateMenu.addItem("Eighth", 4);
    rateMenu.addItem("Sixteenth", 5);
    rateMenu.addItem("Thirty-Second", 6);
    rateMenu.addItem("Sixty-Fourth", 7);
    addAndMakeVisible(rateMenu);
    rateAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "rate", rateMenu);

    rateTypeMenu.addItem("Straight", 1);
    rateTypeMenu.addItem("Triplet", 2);
    rateTypeMenu.addItem("Dotted", 3);
    addAndMakeVisible(rateTypeMenu);
    rateTypeAttachment = std::make_unique<ComboBoxAttachment>(state.parameters, "rateType", rateTypeMenu);
}

void Header::paint(juce::Graphics &g) {
    g.fillAll(props::COLOR_HEADER);
}

void Header::resized() {
    auto area = getLocalBounds();

    timecodeDisplayLabel.setBounds(area.removeFromTop(26));

    auto bottom = area.removeFromBottom(48).reduced(8);
    const int MENU_HEIGHT = 24;
    juce::FlexBox bottomBox;
    bottomBox.alignItems = juce::FlexBox::AlignItems::center;
    bottomBox.items.add(juce::FlexItem(stepsMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(90));
    bottomBox.items.add(juce::FlexItem(voicesMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(90));
    bottomBox.items.add(juce::FlexItem(rateMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130));
    bottomBox.items.add(juce::FlexItem(rateTypeMenu).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            MENU_HEIGHT).withWidth(130));
    bottomBox.items.add(juce::FlexItem(stretchButton).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
            static_cast<float>(ICON_SIZE + 10)).withWidth(
            static_cast<float>(stretchButton.getWidth() + 15)));
    bottomBox.items.add(
            juce::FlexItem(randomButton).withAlignSelf(juce::FlexItem::AlignSelf::autoAlign).withHeight(
                    static_cast<float>(ICON_SIZE + 10)).withWidth(
                    static_cast<float>(randomButton.getWidth() + 15)));
    bottomBox.performLayout(bottom);
}

void Header::timerCallback() {
    juce::String newText = updateTimecodeDisplay(pluginProcessor.lastPosInfo.get());
    timecodeDisplayLabel.setText(newText, juce::dontSendNotification);
}
