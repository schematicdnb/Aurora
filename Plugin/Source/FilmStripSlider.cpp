/*
  ==============================================================================

    FilmStripSlider.cpp
    Created: 11 Mar 2025 5:15:07pm
    Author:  Dan Dubois

  ==============================================================================
*/

#include "FilmStripSlider.h"

FilmStripSlider::FilmStripSlider(juce::Image *_knobStrip, int _frameCount, int _frameSize, bool _isVerticalStrip)
{
    knobStrip = _knobStrip;
    frameSize = _frameSize;
    frameCount = _frameCount;
    isVerticalStrip = _isVerticalStrip;
}

void FilmStripSlider::drawFrame(juce::Graphics &g, int x, int y, int width, int height, juce::Slider &slider)
{
    // Adjust the slider value to account for the skew factor
    const double skewFactor = slider.getSkewFactor();
    const double sliderValue = slider.getValue();
    const double sliderMin = slider.getMinimum();
    const double sliderMax = slider.getMaximum();
    double adjustedValue;

    if (skewFactor != 1.0)
    {
        adjustedValue = sliderMin + (std::pow((sliderValue - sliderMin) / (sliderMax - sliderMin), skewFactor) * (sliderMax - sliderMin));
    }
    else
    {
        adjustedValue = sliderValue;
    }

    const double fractRotation = (adjustedValue - sliderMin) / (sliderMax - sliderMin);
    int pos = (int)ceil(fractRotation * ((double)frameCount - 1.0));

    if (width != height)
    {
        x = (width / 2) - (height / 2);
        width = height;
    }

    if (isVerticalStrip)
    {
        g.drawImage(*knobStrip, x, y, width, height, 0, (int)(pos * frameSize), frameSize, frameSize, false);
    }
    else
    {
        g.drawImage(*knobStrip, x, y, width, height, (int)(pos * frameSize), 0, frameSize, frameSize, false);
    }
}
