/*
  ==============================================================================

    SchematicLookAndFeel.cpp
    Created: 11 Mar 2025 5:03:50pm
    Author:  Dan Dubois

  ==============================================================================
*/

#include "SchematicLookAndFeel.h"

void SchematicLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    fss.drawFrame(g, x, y, width, height, slider);
}

void SchematicLookAndFeel::setTheme(Theme t) {
    theme = t;
    
    if (theme == Theme::Dark) { // Dark theme
        setColour(juce::ResizableWindow::backgroundColourId, Colour(24,24,24));
        setColour(GroupComponent::ColourIds::textColourId, Colour(200,200,200));
        setColour(Slider::ColourIds::textBoxTextColourId, Colour(100,100,100)); // text
        setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(232,232,232)); // outline
        setColour(Label::ColourIds::textColourId, Colour(232,232,232));
        setColour(ToggleButton::tickColourId, Colour(232,232,232));
        setColour(ToggleButton::tickDisabledColourId, Colour(232,232,232));
    } else { // Light theme
        setColour(juce::ResizableWindow::backgroundColourId, Colour(232,232,232));
        setColour(GroupComponent::ColourIds::textColourId, Colour(32,32,32));
        setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
        setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32)); // outline
        setColour(Label::ColourIds::textColourId, Colour(32,32,32));
        setColour(ToggleButton::tickColourId, Colour(32,32,32));
        setColour(ToggleButton::tickDisabledColourId, Colour(32,32,32));
    }
}
