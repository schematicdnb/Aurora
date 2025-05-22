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

void SchematicLookAndFeel::toggleTheme() {
    if (darkMode) {
        
        darkMode = false;
    } else {
        
        darkMode = true;
    }
}
