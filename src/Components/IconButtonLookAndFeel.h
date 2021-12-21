#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "FontAwesome.h"

class IconButtonLookAndFeel : public juce::LookAndFeel_V4 {
public:
    IconButtonLookAndFeel(juce::String charCode, float size) {
        icon = FontAwesome::getInstance()->getIcon(charCode, size, juce::Colours::red, 1);
        iconDown = FontAwesome::getInstance()->getIcon(charCode, size, juce::Colours::white, 1);
    }

    void drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &, bool,
                              bool shouldDrawButtonAsDown) override {
        int iconWidth = icon.getWidth();
        int iconHeight = icon.getHeight();
        auto area = b.getLocalBounds();
        g.drawImage(shouldDrawButtonAsDown ? iconDown : icon,
                    static_cast<int>(std::ceil((area.getWidth() - iconWidth) / 2)),
                    static_cast<int>(std::ceil((area.getHeight() - iconHeight) / 2)),
                    iconWidth, iconHeight,
                    0, 0,
                    iconWidth, iconHeight,
                    false);
    }

private:
    juce::Image icon;
    juce::Image iconDown;
};
