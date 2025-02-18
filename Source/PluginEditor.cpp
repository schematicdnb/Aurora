/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RGBMeterAudioProcessorEditor::RGBMeterAudioProcessorEditor(RGBMeterAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

//  addAndMakeVisible(avc);

  // make components visible
  addAndMakeVisible(rgbMeter);

  addAndMakeVisible(lowCrossoverSlider);
  addAndMakeVisible(highCrossoverSlider);
  addAndMakeVisible(lowEnableButton);
  addAndMakeVisible(midEnableButton);
  addAndMakeVisible(highEnableButton);

  // set slider ranges
  auto lowRange = new juce::NormalisableRange<double>(20, 999, 1, 1);
  auto highRange = new juce::NormalisableRange<double>(1000, 20000, 1, 1);
  lowCrossoverSlider.setNormalisableRange(*lowRange);
  highCrossoverSlider.setNormalisableRange(*highRange);

  // set slider attachments
  lowCrossoverAttachment.reset(new SliderAttachment(apvts, "lowCrossover", lowCrossoverSlider));
  highCrossoverAttachment.reset(new SliderAttachment(apvts, "highCrossover", highCrossoverSlider));

  // set button attachments
  lowButtonAttachment.reset(new ButtonAttachment(apvts, "enableLow", lowEnableButton));
  midButtonAttachment.reset(new ButtonAttachment(apvts, "enableMid", midEnableButton));
  highButtonAttachment.reset(new ButtonAttachment(apvts, "enableHigh", highEnableButton));

  // set button text
  lowEnableButton.setButtonText("Low");
  midEnableButton.setButtonText("Mid");
  highEnableButton.setButtonText("High");

  // set button toggle and colours
  lowEnableButton.setClickingTogglesState(true);
  lowEnableButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
  lowEnableButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);

  midEnableButton.setClickingTogglesState(true);
  midEnableButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
  midEnableButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);

  highEnableButton.setClickingTogglesState(true);
  highEnableButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
  highEnableButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);

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

  setSize(800, 500);
}

RGBMeterAudioProcessorEditor::~RGBMeterAudioProcessorEditor()
{
}

//==============================================================================
void RGBMeterAudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  //    g.setFont (juce::FontOptions (15.0f));
  //    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void RGBMeterAudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  rgbMeter.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() * 0.75, getHeight() * 0.33));
  avc.setBounds(rgbMeter.getX(), rgbMeter.getY() - rgbMeter.getHeight(), rgbMeter.getWidth(), rgbMeter.getHeight());

  lowCrossoverSlider.setBounds(10, 10, getWidth() / 2, 20);
  highCrossoverSlider.setBounds(10, 30, getWidth() / 2, 20);
  lowEnableButton.setBounds(rgbMeter.getX(), rgbMeter.getY() + rgbMeter.getHeight() + 10, rgbMeter.getWidth() / 3, 20);
  midEnableButton.setBounds(lowEnableButton.getX() + lowEnableButton.getWidth(), lowEnableButton.getY(), lowEnableButton.getWidth(), lowEnableButton.getHeight());
  highEnableButton.setBounds(midEnableButton.getX() + lowEnableButton.getWidth(), lowEnableButton.getY(), lowEnableButton.getWidth(), lowEnableButton.getHeight());
}
