#include "juce_gui_basics/juce_gui_basics.h"

#include "FontAwesome.h"

FontAwesome::FontAwesome() {
}

FontAwesome::~FontAwesome() {
    clearSingletonInstance();
}

JUCE_IMPLEMENT_SINGLETON (FontAwesome)

juce::Image FontAwesome::getIcon(juce::String charCode, float size, juce::Colour colour, float scaleFactor) {
    int scaledSize = int(size * scaleFactor);

    juce::String identifier = juce::String(charCode + "@" + juce::String(scaledSize) + "@" + colour.toString());
    juce::int64 hash = identifier.hashCode64();
    juce::Image canvas = juce::ImageCache::getFromHashCode(hash);
    if (canvas.isValid())
        return canvas;

    juce::Font fontAwesome = getFont((float) scaledSize);
    scaledSize = std::max(fontAwesome.getStringWidth(charCode), scaledSize);

    canvas = juce::Image(juce::Image::PixelFormat::ARGB, scaledSize, scaledSize, true);
    juce::Graphics g(canvas);
    g.setColour(colour);
    g.setFont(fontAwesome);
    g.drawText(charCode, 0, 0, scaledSize, scaledSize, juce::Justification::centred, true);
    juce::ImageCache::addImageToCache(canvas, hash);
    return canvas;
}

juce::Font FontAwesome::getFont() {
    static juce::Font fontAwesomeFont(FontAwesome_ptr);
    return fontAwesomeFont;
}

juce::Font FontAwesome::getFont(float size) {
    juce::Font font = getFont();
    font.setHeight(size);
    return font;
}
