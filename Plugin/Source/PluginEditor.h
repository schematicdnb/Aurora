/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SchematicLookAndFeel.h"
#include "UpdateNotifier.h"

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
    void initControlToggle();
    void showControls();
    void hideControls();
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AuroraAudioProcessor& audioProcessor;
    
    int restoreWidth, restoreHeight;
    bool requiresRestore = false;
    
    UpdateNotifier updateNotifier{};
    
    SchematicLookAndFeel customLookAndFeel;
    bool darkTheme = true;
    
    Slider historySlider, gainSlider;
    Label historyLabel, gainLabel;
    
    int margin = 15;
    int paramWidth = 50;
    int paramHeight = 75;
    
    GroupComponent zoomGroup, crossoverGroup, colourGroup;
    int groupWidth = 2*paramWidth + 3*margin;
    int groupHeight = paramHeight + 2*margin;
    int labelHeight = 0;
    int infoAreaHeight = 6*margin;
    
    Slider lowCrossoverSlider, highCrossoverSlider;
    Label lowCrossoverLabel, highCrossoverLabel;
    
    Slider redSlider, greenSlider, blueSlider;
    Label redLabel, greenLabel, blueLabel;
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    std::unique_ptr<SliderAttachment> historySliderAttachment, gainSliderAttachment;
    std::unique_ptr<SliderAttachment> lowCrossoverAttachment, highCrossoverAttachment;
    std::unique_ptr<SliderAttachment> redSliderAttachment, greenSliderAttachment, blueSliderAttachment;
    
    AudioProcessorValueTreeState &apvts = audioProcessor.apvts;
    RGBMeter &rgbMeter = audioProcessor.rgbMeter;
    
    ToggleButton toggleControlsButton;
    Label toggleControlsLabel;
    std::unique_ptr<ButtonAttachment> toggleControlsAttachment;
    
    TextButton channelButton;
    std::unique_ptr<ButtonAttachment> channelButtonAttachment;
    
    Array<Component*> controls = {&historySlider, &gainSlider, &historyLabel, &gainLabel, &zoomGroup, &crossoverGroup, &colourGroup, &lowCrossoverSlider, &highCrossoverSlider, &lowCrossoverLabel, &highCrossoverLabel, &redSlider, &greenSlider, &blueSlider, &redLabel, &greenLabel, &blueLabel};
    
    Image logoAurora = ImageCache::getFromMemory(BinaryData::AuroraLogoLightMode_png, BinaryData::AuroraLogoLightMode_pngSize);

    Image logoSchematicBlack = ImageCache::getFromMemory(BinaryData::SchematicSoundLogoBlack_png, BinaryData::SchematicSoundLogoBlack_pngSize);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuroraAudioProcessorEditor)
};
