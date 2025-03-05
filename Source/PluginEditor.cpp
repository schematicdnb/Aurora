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
    
    // Gain parameter
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    gainSlider.setSize(paramWidth, paramHeight);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    gainSlider.setTextValueSuffix(" dB");
    
    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("Gain", dontSendNotification);
    gainLabel.setJustificationType(Justification::centred);
    
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
    
    addAndMakeVisible(historyLabel);
    historyLabel.attachToComponent(&historySlider, false);
    historyLabel.setText("History", dontSendNotification);
    historyLabel.setJustificationType(Justification::centred);
    
    historySliderAttachment.reset(new SliderAttachment(apvts, "historyLength", historySlider));
    
    historySlider.onValueChange = [this]() {
        rgbMeter.setHistoryLength(historySlider.getValue());
        rgbMeter.updateState();
    };
    
    // Crossover Group
    addAndMakeVisible(crossoverGroup);
    crossoverGroup.setText("Crossovers");
    crossoverGroup.setTextLabelPosition(Justification::centredBottom);
    
    // Low Crossover
    addAndMakeVisible(lowCrossoverSlider);
    lowCrossoverSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    lowCrossoverSlider.setSize(paramWidth, paramHeight);
    lowCrossoverSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    lowCrossoverSlider.setTextValueSuffix(" Hz");
    
    addAndMakeVisible(lowCrossoverLabel);
    lowCrossoverLabel.attachToComponent(&lowCrossoverSlider, false);
    lowCrossoverLabel.setText("Low", dontSendNotification);
    lowCrossoverLabel.setJustificationType(Justification::centred);
    
    lowCrossoverAttachment.reset(new SliderAttachment(apvts, "lowCrossover", lowCrossoverSlider));
    
    lowCrossoverSlider.onValueChange = [this]() {
        auto value = lowCrossoverSlider.getValue();
        auto limit = highCrossoverSlider.getValue();
        if (value >= limit) {
            value = limit - 1;
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
    
    addAndMakeVisible(highCrossoverLabel);
    highCrossoverLabel.attachToComponent(&highCrossoverSlider, false);
    highCrossoverLabel.setText("High", dontSendNotification);
    highCrossoverLabel.setJustificationType(Justification::centred);
    
    highCrossoverAttachment.reset(new SliderAttachment(apvts, "highCrossover", highCrossoverSlider));
    
    highCrossoverSlider.onValueChange = [this]() {
        auto value = highCrossoverSlider.getValue();
        auto limit = lowCrossoverSlider.getValue();
        if (value <= limit) {
            value = limit - 1;
            highCrossoverSlider.setValue(value);
        }
        rgbMeter.setHighCrossover(value);
    };
    
    // Set window size
    setSize(audioProcessor.getEditorWidth(), audioProcessor.getEditorHeight());
    
    // make window resizable
    setResizable(true, true);
    setResizeLimits(640, 250, 1280, 720);
    
    
//  addAndMakeVisible(lowCrossoverSlider);
//  addAndMakeVisible(highCrossoverSlider);
//  addAndMakeVisible(lowEnableButton);
//  addAndMakeVisible(midEnableButton);
//  addAndMakeVisible(highEnableButton);

//  // set slider ranges
//  auto lowRange = new juce::NormalisableRange<double>(20, 999, 1, 1);
//  auto highRange = new juce::NormalisableRange<double>(1000, 20000, 1, 1);
//  lowCrossoverSlider.setNormalisableRange(*lowRange);
//  highCrossoverSlider.setNormalisableRange(*highRange);
//
//  // set slider attachments
//  lowCrossoverAttachment.reset(new SliderAttachment(apvts, "lowCrossover", lowCrossoverSlider));
//  highCrossoverAttachment.reset(new SliderAttachment(apvts, "highCrossover", highCrossoverSlider));
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

  // initialize values from parameters
  //    lowCrossoverSlider.setValue(audioProcessor.lowCrossover->get());
  //    highCrossoverSlider.setValue(audioProcessor.highCrossover->get());
  //
  //    lowEnableButton.setToggleState(audioProcessor.enableLow->get(), juce::sendNotification);
  //    midEnableButton.setToggleState(audioProcessor.enableMid->get(), juce::sendNotification);
  //    highEnableButton.setToggleState(audioProcessor.enableHigh->get(), juce::sendNotification);

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
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::black);
    
    auto labelHeight = gainLabel.getHeight();

    g.fillRoundedRectangle(margin, margin, getWidth() - 2*margin, getHeight() - groupHeight - labelHeight - 3*margin, 0);

  
}

void AuroraAudioProcessorEditor::resized(){
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
    
    auto labelHeight = gainLabel.getHeight();
    audioProcessor.setEditorSize(getWidth(), getHeight());
    
    rgbMeter.setBounds(margin, margin, getWidth() - 2*margin, getHeight() - 3*margin - groupHeight - labelHeight);
    
    // Zoom controls
    zoomGroup.setBounds(getWidth() - margin - groupWidth, getHeight() - margin - groupHeight - labelHeight, groupWidth, groupHeight + labelHeight);
    
    historySlider.setBounds(zoomGroup.getX() + margin, zoomGroup.getY() + labelHeight + margin, paramWidth, paramHeight);
    
    gainSlider.setBounds(historySlider.getX() + paramWidth + margin, historySlider.getY(), paramWidth, paramHeight);
    
    // Crossover controls
    crossoverGroup.setBounds(margin, zoomGroup.getY(), groupWidth, groupHeight + labelHeight);
    
    lowCrossoverSlider.setBounds(crossoverGroup.getX() + margin, gainSlider.getY(), paramWidth, paramHeight);
    
    highCrossoverSlider.setBounds(lowCrossoverSlider.getX() + paramWidth + margin, lowCrossoverSlider.getY(), paramWidth, paramHeight);
    
    
    



//  lowCrossoverSlider.setBounds(10, 10, getWidth() / 2, 20);
//  highCrossoverSlider.setBounds(10, 30, getWidth() / 2, 20);
//    lowEnableButton.setBounds(0, getHeight() - 20, getWidth() / 3, 20);
//midEnableButton.setBounds(lowEnableButton.getX() + lowEnableButton.getWidth(), lowEnableButton.getY(), lowEnableButton.getWidth(), lowEnableButton.getHeight());
//highEnableButton.setBounds(midEnableButton.getX() + lowEnableButton.getWidth(), lowEnableButton.getY(), lowEnableButton.getWidth(), lowEnableButton.getHeight());
}
