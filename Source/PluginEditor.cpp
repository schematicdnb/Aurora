/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AuroraAudioProcessorEditor::AuroraAudioProcessorEditor(AuroraAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
    
    // add meter
    addAndMakeVisible(rgbMeter);
    
    // Zoom Group
    addAndMakeVisible(zoomGroup);
    zoomGroup.setText("Zoom");
    zoomGroup.setTextLabelPosition(Justification::centredBottom);
    zoomGroup.setColour(GroupComponent::ColourIds::textColourId, Colour(32,32,32));
    
    // Gain parameter
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    gainSlider.setSize(paramWidth, paramHeight);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    gainSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));

    
    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("Gain", dontSendNotification);
    gainLabel.setJustificationType(Justification::centred);
    gainLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    gainSliderAttachment.reset(new SliderAttachment(apvts, "gain", gainSlider));

    gainSlider.onValueChange = [this]() {
        rgbMeter.setGain(gainSlider.getValue());
    };
    
    // History parameter
    addAndMakeVisible(historySlider);
    historySlider.setSliderStyle(Slider::RotaryVerticalDrag);
    historySlider.setSize(paramWidth, paramHeight);
    historySlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    historySlider.setTextValueSuffix(" sec");
    historySlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    historySlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
    
    addAndMakeVisible(historyLabel);
    historyLabel.attachToComponent(&historySlider, false);
    historyLabel.setText("History", dontSendNotification);
    historyLabel.setJustificationType(Justification::centred);
    historyLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    historySliderAttachment.reset(new SliderAttachment(apvts, "historyLength", historySlider));
    
    historySlider.onValueChange = [this]() {
        rgbMeter.setHistoryLength(historySlider.getValue());
        rgbMeter.updateState();
    };
    
    // Crossover Group
    addAndMakeVisible(crossoverGroup);
    crossoverGroup.setText("Crossovers");
    crossoverGroup.setTextLabelPosition(Justification::centredBottom);
    crossoverGroup.setColour(GroupComponent::ColourIds::textColourId, Colour(32,32,32));
    
    // Low Crossover
    addAndMakeVisible(lowCrossoverSlider);
    lowCrossoverSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    lowCrossoverSlider.setSize(paramWidth, paramHeight);
    lowCrossoverSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    lowCrossoverSlider.setTextValueSuffix(" Hz");
    lowCrossoverSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    lowCrossoverSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
    
    addAndMakeVisible(lowCrossoverLabel);
    lowCrossoverLabel.attachToComponent(&lowCrossoverSlider, false);
    lowCrossoverLabel.setText("Low", dontSendNotification);
    lowCrossoverLabel.setJustificationType(Justification::centred);
    lowCrossoverLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    lowCrossoverAttachment.reset(new SliderAttachment(apvts, "lowCrossover", lowCrossoverSlider));
    
    lowCrossoverSlider.onValueChange = [this]() {
        auto value = lowCrossoverSlider.getValue();
        auto limit = highCrossoverSlider.getValue();
        if (value >= limit) {
            value = limit;
            lowCrossoverSlider.setValue(value);
        }
        rgbMeter.setLowCrossover(value);
    };
    
    // High Crossover
    addAndMakeVisible(highCrossoverSlider);
    highCrossoverSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    highCrossoverSlider.setSize(paramWidth, paramHeight);
    highCrossoverSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    highCrossoverSlider.setTextValueSuffix(" Hz");
    highCrossoverSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    highCrossoverSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
    
    addAndMakeVisible(highCrossoverLabel);
    highCrossoverLabel.attachToComponent(&highCrossoverSlider, false);
    highCrossoverLabel.setText("High", dontSendNotification);
    highCrossoverLabel.setJustificationType(Justification::centred);
    highCrossoverLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    highCrossoverAttachment.reset(new SliderAttachment(apvts, "highCrossover", highCrossoverSlider));
    
    highCrossoverSlider.onValueChange = [this]() {
        auto value = highCrossoverSlider.getValue();
        auto limit = lowCrossoverSlider.getValue();
        if (value <= limit) {
            value = limit;
            highCrossoverSlider.setValue(value);
        }
        rgbMeter.setHighCrossover(value);
    };
    
    // Set window size
    setSize(audioProcessor.getEditorWidth(), audioProcessor.getEditorHeight());
    
    // make window resizable
    setResizable(true, true);
    setResizeLimits(640, 250, 1280, 720);
    
    

//  addAndMakeVisible(lowEnableButton);
//  addAndMakeVisible(midEnableButton);
//  addAndMakeVisible(highEnableButton);

//
//  // set button attachments
//  lowButtonAttachment.reset(new ButtonAttachment(apvts, "enableLow", lowEnableButton));
//  midButtonAttachment.reset(new ButtonAttachment(apvts, "enableMid", midEnableButton));
//  highButtonAttachment.reset(new ButtonAttachment(apvts, "enableHigh", highEnableButton));
//
//  // set button text
//  lowEnableButton.setButtonText("Low");
//  midEnableButton.setButtonText("Mid");
//  highEnableButton.setButtonText("High");
//
//  // set button toggle and colours
//  lowEnableButton.setClickingTogglesState(true);
//  lowEnableButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
//  lowEnableButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
//
//  midEnableButton.setClickingTogglesState(true);
//  midEnableButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
//  midEnableButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
//
//  highEnableButton.setClickingTogglesState(true);
//  highEnableButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
//  highEnableButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);


  // // Set change functions
  // lowEnableButton.onClick = [this]() {
  //   *audioProcessor.enableLow = lowEnableButton.getToggleState();
  // };
  // midEnableButton.onClick = [this]() {
  //   *audioProcessor.enableMid = midEnableButton.getToggleState();
  // };
  // highEnableButton.onClick = [this]() {
  //   *audioProcessor.enableHigh = highEnableButton.getToggleState();
  // };


}

AuroraAudioProcessorEditor::~AuroraAudioProcessorEditor()
{
}

//==============================================================================
void AuroraAudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll(Colour(32,32,32));
    g.fillAll(Colour(232, 232, 232));
    
    auto labelHeight = gainLabel.getHeight();

    g.setColour(Colours::black);
    g.fillRoundedRectangle(margin, margin, getWidth() - 2*margin, getHeight() - groupHeight - labelHeight - 3*margin, rgbMeter.getCornerRadius());

  
}

void AuroraAudioProcessorEditor::resized(){
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
    
    auto labelHeight = gainLabel.getHeight();
    audioProcessor.setEditorSize(getWidth(), getHeight());
    
    rgbMeter.setBounds(margin, margin, getWidth() - 2*margin, getHeight() - 3*margin - groupHeight - labelHeight);
    
    // Zoom controls
    zoomGroup.setBounds(getWidth() - margin+2 - groupWidth, getHeight() - margin - groupHeight - labelHeight, groupWidth, groupHeight + labelHeight);
    
    historySlider.setBounds(zoomGroup.getX() + margin, zoomGroup.getY() + labelHeight + margin, paramWidth, paramHeight);
    
    gainSlider.setBounds(historySlider.getX() + paramWidth + margin, historySlider.getY(), paramWidth, paramHeight);
    
    // Crossover controls
    crossoverGroup.setBounds(margin-2, zoomGroup.getY(), groupWidth, groupHeight + labelHeight);
    
    lowCrossoverSlider.setBounds(crossoverGroup.getX() + margin, gainSlider.getY(), paramWidth, paramHeight);
    
    highCrossoverSlider.setBounds(lowCrossoverSlider.getX() + paramWidth + margin, lowCrossoverSlider.getY(), paramWidth, paramHeight);
    
//    lowEnableButton.setBounds(0, getHeight() - 20, getWidth() / 3, 20);
//midEnableButton.setBounds(lowEnableButton.getX() + lowEnableButton.getWidth(), lowEnableButton.getY(), lowEnableButton.getWidth(), lowEnableButton.getHeight());
//highEnableButton.setBounds(midEnableButton.getX() + lowEnableButton.getWidth(), lowEnableButton.getY(), lowEnableButton.getWidth(), lowEnableButton.getHeight());
}
