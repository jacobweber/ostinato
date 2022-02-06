#include "juce_gui_basics/juce_gui_basics.h"

#include "LookAndFeel.h"
#include "../Constants.h"

LookAndFeel::LookAndFeel() {
    setColour(juce::TextButton::ColourIds::buttonOnColourId, constants::COLOR_TOGGLE_ACTIVE);
    setColour(juce::TextButton::ColourIds::buttonColourId, constants::COLOR_TOGGLE_INACTIVE);
    setColour(juce::TextButton::ColourIds::textColourOnId, constants::COLOR_TOGGLE_ACTIVE_TEXT);
    setColour(juce::TextButton::ColourIds::textColourOffId, constants::COLOR_TOGGLE_INACTIVE_TEXT);
    setColour(juce::ComboBox::ColourIds::backgroundColourId, constants::COLOR_COMBO_BG);
    setColour(juce::PopupMenu::ColourIds::backgroundColourId, constants::COLOR_COMBO_BG);
}

void LookAndFeel::drawButtonBackground(juce::Graphics &g,
                            juce::Button &button,
                            const juce::Colour &backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour(baseColour);

    g.fillRect(bounds);
}

void LookAndFeel::drawImageButton(juce::Graphics &g, juce::Image *image,
                        int imageX, int imageY, int imageW, int imageH,
                        const juce::Colour &overlayColour,
                        float imageOpacity,
                        juce::ImageButton &button) {
    juce::LookAndFeel_V4::drawImageButton(g, image, imageX, imageY, imageW, imageH, overlayColour, imageOpacity,
                                            button);
}
