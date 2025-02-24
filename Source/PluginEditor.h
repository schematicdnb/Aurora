/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RGBMeterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RGBMeterAudioProcessorEditor (RGBMeterAudioProcessor&);
    ~RGBMeterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RGBMeterAudioProcessor& audioProcessor;
    
    Slider historySlider();
    Slider gainSlider;
    
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    std::unique_ptr<SliderAttachment> historySliderAttachment, gainSliderAttachment;
    
//    juce::Slider lowCrossoverSlider;
//    juce::Slider highCrossoverSlider;
//    juce::TextButton lowEnableButton;
//    juce::TextButton midEnableButton;
//    juce::TextButton highEnableButton;
    
    juce::AudioProcessorValueTreeState& apvts = audioProcessor.apvts;
    juce::RGBMeter& rgbMeter = audioProcessor.rgbMeter;
    
//    std::unique_ptr<SliderAttachment> lowCrossoverAttachment, highCrossoverAttachment;
//    std::unique_ptr<ButtonAttachment> lowButtonAttachment, midButtonAttachment, highButtonAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RGBMeterAudioProcessorEditor)
};
