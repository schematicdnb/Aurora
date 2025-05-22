/*
  ==============================================================================

    SchematicLookAndFeel.h
    Created: 11 Mar 2025 5:03:36pm
    Author:  Dan Dubois

  ==============================================================================
*/
#include <JuceHeader.h>
#include "FilmStripSlider.h"

class SchematicLookAndFeel : public juce::LookAndFeel_V4
{
    public:
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
    
    void toggleTheme();
    
    private:
    Image schematicKnob = ImageCache::getFromMemory(BinaryData::AuroraKnob_png, BinaryData::AuroraKnob_pngSize);
    FilmStripSlider fss = FilmStripSlider(&schematicKnob, 64, 64, true);
    
    bool darkMode = false;

    //==============================================================================

    JUCE_LEAK_DETECTOR (SchematicLookAndFeel);
};
