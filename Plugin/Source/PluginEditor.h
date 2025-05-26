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
 * The default moonbase ui integration allows to set a company logo as a component,
 * so you can define custom animations and behaviours.
 *
 * You can change the ANIMATE_COMPANY_LOGO flag below to 1 to enable an example animation that makes the logo shiver.
*/

#define ANIMATE_COMPANY_LOGO 0

class CompanyLogo : public Component,
                    private Timer
{
public:
    CompanyLogo ();
private:
    std::unique_ptr<Drawable> logo;
    void paint (Graphics& g) override;
    
    void timerCallback () override;
    LinearSmoothedValue<float> jitterX { 0.f };
    LinearSmoothedValue<float> jitterY { 0.f };
    Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompanyLogo)
};

//==============================================================================
/**
*/
class AuroraAudioProcessorEditor  : public juce::AudioProcessorEditor, private Moonbase::JUCEClient::ActivationUI::Listener
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
    
    void onActivationUiVisibilityChanged (const ActivationUI::Visibility& visibility) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AuroraAudioProcessor& audioProcessor;
    
    std::unique_ptr<Moonbase::JUCEClient::ActivationUI> activationUI { audioProcessor.moonbaseClient->createActivationUi(*this)
    };
    int restoreWidth, restoreHeight;
    bool requiresRestore;
    
    UpdateNotifier updateNotifier = UpdateNotifier(audioProcessor);
    
    SchematicLookAndFeel customLookAndFeel;
    
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
    
    Image logoAurora = gin::applyResize(ImageCache::getFromMemory(BinaryData::AuroraLogoLightMode_png, BinaryData::AuroraLogoLightMode_pngSize), 0.09);

    Image logoSchematic = gin::applyResize(ImageCache::getFromMemory(BinaryData::SchematicSoundLogo_png, BinaryData::SchematicSoundLogo_pngSize), 0.055);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuroraAudioProcessorEditor)
};
