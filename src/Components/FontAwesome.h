#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "BinaryData.h"

// adapted from https://github.com/danlin/danlin_modules
class FontAwesome : juce::DeletedAtShutdown {
public:
    FontAwesome() = default;

    ~FontAwesome() override;

    JUCE_DECLARE_SINGLETON (FontAwesome, false)

    juce::Image getIcon(bool solid, const juce::String &charCode, float size, juce::Colour colour, float scaleFactor);

    juce::Font getSolidFont();

    juce::Font getRegularFont();

private:
    juce::Typeface::Ptr FontAwesomeRegular_ptr = juce::Typeface::createSystemTypefaceFor(
            BinaryData::fontAwestomeRegular_otf,
            BinaryData::fontAwestomeRegular_otfSize);
    juce::Typeface::Ptr FontAwesomeSolid_ptr = juce::Typeface::createSystemTypefaceFor(BinaryData::fontAwesomeSolid_otf,
                                                                                       BinaryData::fontAwesomeSolid_otfSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FontAwesome)
};
