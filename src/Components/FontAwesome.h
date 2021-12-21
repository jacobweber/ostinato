#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "BinaryData.h"

// adapted from https://github.com/danlin/danlin_modules
class FontAwesome : juce::DeletedAtShutdown {
public:
    FontAwesome();

    ~FontAwesome();

    JUCE_DECLARE_SINGLETON (FontAwesome, false)

    juce::Image getIcon(juce::String charCode, float size, juce::Colour colour, float scaleFactor);

    juce::Font getFont();

    juce::Font getFont(float size);

private:
    juce::Typeface::Ptr FontAwesome_ptr = juce::Typeface::createSystemTypefaceFor(BinaryData::fontawesome_otf,
                                                                                  BinaryData::fontawesome_otfSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FontAwesome)
};
