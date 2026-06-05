/*
  ==============================================================================

    AuroraLookAndFeel.cpp
    Created: 11 Mar 2025 5:03:50pm
    Author:  Dan Dubois

  ==============================================================================
*/

#include "AuroraLookAndFeel.h"

void AuroraLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    fss.drawFrame(g, x, y, width, height, slider);
}

void AuroraLookAndFeel::setTheme(Theme t) {
    theme = t;
    
    if (theme == Theme::Dark) { // Dark theme
        // Background
        setColour(juce::ResizableWindow::backgroundColourId, Colour(32,32,32));
        // Group outline
        setColour(GroupComponent::ColourIds::textColourId, Colour(232,232,232));
        // Sliders/knobs
        setColour(Slider::ColourIds::textBoxTextColourId, Colour(232,232,232));
        setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(232,232,232));
        // Labels
        setColour(Label::ColourIds::textColourId, Colour(232,232,232));
        // Buttons
        setColour(TextButton::buttonColourId, Colour(32,32,32));
        setColour(TextButton::textColourOnId, Colour(232,232,232));
        setColour(TextButton::textColourOffId, Colour(232,232,232));
        setColour(TextButton::buttonOnColourId, Colour(32,32,32));
    } else { // Light theme
        // Background
        setColour(juce::ResizableWindow::backgroundColourId, Colour(232,232,232));
        // Group outline
        setColour(GroupComponent::ColourIds::textColourId, Colour(32,32,32));
        // Sliders/knobs
        setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
        setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
        // Labels
        setColour(Label::ColourIds::textColourId, Colour(32,32,32));
        // Buttons
        setColour(TextButton::buttonColourId, Colour(232,232,232));
        setColour(TextButton::textColourOnId, Colour(32,32,32));
        setColour(TextButton::textColourOffId, Colour(32,32,32));
        setColour(TextButton::buttonOnColourId, Colour(232,232,232));
    }
}

AuroraLookAndFeel::Theme AuroraLookAndFeel::getTheme() {
    return theme;
}
