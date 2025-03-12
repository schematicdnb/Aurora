/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SchematicLookAndFeel.h"

//==============================================================================
/**
*/
class AuroraAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AuroraAudioProcessorEditor (AuroraAudioProcessor&);
    ~AuroraAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void initZoomGroup();
    void initCrossoverGroup();
    void initColourGroup();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AuroraAudioProcessor& audioProcessor;
    SchematicLookAndFeel customLookAndFeel;
    
    Slider historySlider, gainSlider;
    Label historyLabel, gainLabel;
    
    int margin = 15;
    int paramWidth = 50;
    int paramHeight = 75;
    
    GroupComponent zoomGroup, crossoverGroup, colourGroup;
    int groupWidth = 2*paramWidth + 3*margin;
    int groupHeight = paramHeight + 2*margin;
    
    Slider lowCrossoverSlider, highCrossoverSlider;
    Label lowCrossoverLabel, highCrossoverLabel;
    
    Slider redSlider, greenSlider, blueSlider;
    Label redLabel, greenLabel, blueLabel;
    
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    std::unique_ptr<SliderAttachment> historySliderAttachment, gainSliderAttachment;
    std::unique_ptr<SliderAttachment> lowCrossoverAttachment, highCrossoverAttachment;
    std::unique_ptr<SliderAttachment> redSliderAttachment, greenSliderAttachment, blueSliderAttachment;
    
    juce::AudioProcessorValueTreeState &apvts = audioProcessor.apvts;
    juce::RGBMeter &rgbMeter = audioProcessor.rgbMeter;
    
//    ToggleButton darkModeButton;
    TextButton darkModeButton;
    Label themeLabel;
    std::unique_ptr<ButtonAttachment> darkModeAttachment;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuroraAudioProcessorEditor)
};
