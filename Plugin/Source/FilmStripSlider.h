/*
  ==============================================================================

    FilmStripSlider.h
    Created: 11 Mar 2025 5:15:00pm
    Author:  Dan Dubois

  ==============================================================================
*/

#include <JuceHeader.h>

class FilmStripSlider
{
    public:
    FilmStripSlider(juce::Image* _knobStrip, int _frameCount, int _frameSize, bool _isVerticalStrip);
    void drawFrame(juce::Graphics& g, int x, int y, int width, int height, juce::Slider& slider);
    
    private:
    juce::Image* knobStrip = nullptr;
    int frameCount = 0;
    int frameSize = 0;
    bool isVerticalStrip = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilmStripSlider)
};
