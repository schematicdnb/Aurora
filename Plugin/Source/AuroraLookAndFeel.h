/*
  ==============================================================================

    AuroraLookAndFeel.h
    Created: 11 Mar 2025 5:03:36pm
    Author:  Dan Dubois

  ==============================================================================
*/
#include <JuceHeader.h>
#include "FilmStripSlider.h"

class AuroraLookAndFeel : public juce::LookAndFeel_V4
{
    public:
    
    enum class Theme {
        Light,
        Dark,
    };
    
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
    
    void setTheme(Theme t);
    Theme getTheme();
    
    private:
    Image schematicKnob = ImageCache::getFromMemory(BinaryData::AuroraKnob_png, BinaryData::AuroraKnob_pngSize);
    FilmStripSlider fss = FilmStripSlider(&schematicKnob, 64, 64, true);

    Theme theme;

    //==============================================================================

    JUCE_LEAK_DETECTOR (AuroraLookAndFeel);
};
