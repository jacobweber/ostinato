#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "../Constants.h"

class LookAndFeel : public juce::LookAndFeel_V4 {
public:
    LookAndFeel();

private:
    void drawButtonBackground(juce::Graphics &g,
                              juce::Button &button,
                              const juce::Colour &backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawImageButton(juce::Graphics &g, juce::Image *image,
                         int imageX, int imageY, int imageW, int imageH,
                         const juce::Colour &overlayColour,
                         float imageOpacity,
                         juce::ImageButton &button) override;
};