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

  addAndMakeVisible(audioProcessor.rgbMeter);
  // juce::Slider lowCrossoverSlider;

  // lowCrossoverSlider.addListener(this);
  addAndMakeVisible(lowCrossoverSlider);
  //    lowCrossoverSlider.set
  // using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  // auto lowCrossoverAttachment = std::unique_ptr<Attachment>(p.apvts, "lowCrossover", lowCrossoverSlider);

  lowCrossoverSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  lowCrossoverSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
  lowCrossoverSlider.setRange(20.0, 999.0, 1.0);
  // lowCrossoverSlider.setValue(static_cast<double>(p.lowCrossover));
//   lowCrossoverSlider.setValue(audioProcessor.lowCrossover);
  lowCrossoverSlider.addListener(this);

  setSize(400, 300);
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

  audioProcessor.rgbMeter.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() * 0.5, getHeight() * 0.5));
  lowCrossoverSlider.setBounds(10, 10, getWidth() - 20, 20);
}

void RGBMeterAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
  if (slider == &lowCrossoverSlider)
  {
    audioProcessor.lowCrossover->setValueNotifyingHost(lowCrossoverSlider.getValue());
  }
}
