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
    
    initZoomGroup();
    initCrossoverGroup();
    initColourGroup();
    
    // Set window size
    setSize(audioProcessor.getEditorWidth(), audioProcessor.getEditorHeight());
    
    // make window resizable
    setResizable(true, true);
    setResizeLimits(640, 250, 1280, 720);


}

AuroraAudioProcessorEditor::~AuroraAudioProcessorEditor()
{
}

void AuroraAudioProcessorEditor::initZoomGroup() {
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

    // Gain Label
    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("Gain", dontSendNotification);
    gainLabel.setJustificationType(Justification::centred);
    gainLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // Gain Attachment
    gainSliderAttachment.reset(new SliderAttachment(apvts, "gain", gainSlider));

    // Gain function
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
    
    // History Label
    addAndMakeVisible(historyLabel);
    historyLabel.attachToComponent(&historySlider, false);
    historyLabel.setText("History", dontSendNotification);
    historyLabel.setJustificationType(Justification::centred);
    historyLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // History Attachment
    historySliderAttachment.reset(new SliderAttachment(apvts, "historyLength", historySlider));
    
    // History function
    historySlider.onValueChange = [this]() {
        rgbMeter.setHistoryLength(historySlider.getValue());
        rgbMeter.updateState();
    };
}
void AuroraAudioProcessorEditor::initCrossoverGroup() {
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
    
    // Low crossover label
    addAndMakeVisible(lowCrossoverLabel);
    lowCrossoverLabel.attachToComponent(&lowCrossoverSlider, false);
    lowCrossoverLabel.setText("Low", dontSendNotification);
    lowCrossoverLabel.setJustificationType(Justification::centred);
    lowCrossoverLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // Low crossover attachment
    lowCrossoverAttachment.reset(new SliderAttachment(apvts, "lowCrossover", lowCrossoverSlider));
    
    // low crossover function
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
    
    // High Crossover label
    addAndMakeVisible(highCrossoverLabel);
    highCrossoverLabel.attachToComponent(&highCrossoverSlider, false);
    highCrossoverLabel.setText("High", dontSendNotification);
    highCrossoverLabel.setJustificationType(Justification::centred);
    highCrossoverLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // High Crossover attachment
    highCrossoverAttachment.reset(new SliderAttachment(apvts, "highCrossover", highCrossoverSlider));
    
    // High Crossover function
    highCrossoverSlider.onValueChange = [this]() {
        auto value = highCrossoverSlider.getValue();
        auto limit = lowCrossoverSlider.getValue();
        if (value <= limit) {
            value = limit;
            highCrossoverSlider.setValue(value);
        }
        rgbMeter.setHighCrossover(value);
    };
}
void AuroraAudioProcessorEditor::initColourGroup() {
    // Colour Group
    addAndMakeVisible(colourGroup);
    colourGroup.setText("Colour Mixer");
    colourGroup.setTextLabelPosition(Justification::centredBottom);
    colourGroup.setColour(GroupComponent::ColourIds::textColourId, Colour(32,32,32));
    
    
    // Red Slider
    addAndMakeVisible(redSlider);
    redSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    redSlider.setSize(paramWidth, paramHeight);
    redSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    redSlider.setTextValueSuffix(" %");
    redSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    redSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
    
    // Red Label
    addAndMakeVisible(redLabel);
    redLabel.attachToComponent(&redSlider, false);
    redLabel.setText("Red", dontSendNotification);
    redLabel.setJustificationType(Justification::centred);
    redLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // Red Attachment
    redSliderAttachment.reset(new SliderAttachment(apvts, "redWeight", redSlider));
    
    // Red function
    redSlider.onValueChange = [this]() {
        rgbMeter.setColourWeight("red", redSlider.getValue());
        DBG(static_cast<float>(*apvts.getRawParameterValue("redWeight")));
    };
    
    
    // Green Slider
    addAndMakeVisible(greenSlider);
    greenSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    greenSlider.setSize(paramWidth, paramHeight);
    greenSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    greenSlider.setTextValueSuffix(" %");
    greenSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    greenSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
    
    // Green Label
    addAndMakeVisible(greenLabel);
    greenLabel.attachToComponent(&greenSlider, false);
    greenLabel.setText("Green", dontSendNotification);
    greenLabel.setJustificationType(Justification::centred);
    greenLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // Green Attachment
    greenSliderAttachment.reset(new SliderAttachment(apvts, "greenWeight", greenSlider));
    
    // Green function
    greenSlider.onValueChange = [this]() {
        rgbMeter.setColourWeight("green", greenSlider.getValue());
        DBG(static_cast<float>(*apvts.getRawParameterValue("greenWeight")));
    };
    
    
    // Blue Slider
    addAndMakeVisible(blueSlider);
    blueSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    blueSlider.setSize(paramWidth, paramHeight);
    blueSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    blueSlider.setTextValueSuffix(" %");
    blueSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colour(32,32,32));
    blueSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(32,32,32));
    
    // Green Label
    addAndMakeVisible(blueLabel);
    blueLabel.attachToComponent(&blueSlider, false);
    blueLabel.setText("Blue", dontSendNotification);
    blueLabel.setJustificationType(Justification::centred);
    blueLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // Green Attachment
    blueSliderAttachment.reset(new SliderAttachment(apvts, "blueWeight", blueSlider));
    
    // Green function
    blueSlider.onValueChange = [this]() {
        rgbMeter.setColourWeight("blue", blueSlider.getValue());
        DBG(static_cast<float>(*apvts.getRawParameterValue("blueWeight")));
    };
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
    
    // Colour controls
    colourGroup.setBounds(getWidth()/2 - 1.5*paramWidth - 2*margin, crossoverGroup.getY(), 3*paramWidth + 4*margin, groupHeight + labelHeight);
    
    redSlider.setBounds(colourGroup.getX() + margin, gainSlider.getY(), paramWidth, paramHeight);
    greenSlider.setBounds(redSlider.getX() + paramWidth + margin, redSlider.getY(), paramWidth, paramHeight);
    blueSlider.setBounds(greenSlider.getX() + paramWidth + margin, greenSlider.getY(), paramWidth, paramHeight);
}
