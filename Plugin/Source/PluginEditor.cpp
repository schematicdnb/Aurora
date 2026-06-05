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
    
    setLookAndFeel(&auroraLookAndFeel);
    
    // init theme control
    initThemeToggle();

    // add RGB Meter
    addAndMakeVisible(rgbMeter);

    // init channel
    initChannelSelect();
    
    // init controls
    initZoomGroup();
    initCrossoverGroup();
    initColourGroup();
    
    labelHeight = gainLabel.getHeight();
    groupHeight += labelHeight;
    
    // Set window size
    setSize(audioProcessor.getEditorWidth(), audioProcessor.getEditorHeight());
    
    // make window resizable
    setResizable(true, true);
    
    // init controls toggle
    initControlToggle();
    
    // Check for updates
    addChildComponent(updateNotifier);
    if (!updateNotifier.isUpdateDismissed()) {
        updateNotifier.checkForUpdates();
        if (updateNotifier.isUpdateAvailable()) {
            updateNotifier.setInfo();
            updateNotifier.setVisible(true);
        }
    }
    
}

AuroraAudioProcessorEditor::~AuroraAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void AuroraAudioProcessorEditor::initThemeToggle() {
    addAndMakeVisible(themeToggleButton);
    themeToggleButton.setToggleable(true);
    themeToggleButton.setClickingTogglesState(true);
    themeToggleAttachment.reset(new ButtonAttachment(apvts, "theme", themeToggleButton));
    themeToggleButton.setButtonText(themeToggleButton.getToggleState() ? "Dark" : "Light");

    themeToggleButton.onClick = [this]() {
        auto val = themeToggleButton.getToggleState();
        themeToggleButton.setButtonText(val ? "Dark" : "Light");
        auroraLookAndFeel.setTheme(val ? AuroraLookAndFeel::Theme::Dark : AuroraLookAndFeel::Theme::Light);
        
        for (auto* child : getChildren())
            if (auto* s = dynamic_cast<juce::Slider*>(child))
                s->lookAndFeelChanged();
         
        repaint();
    };
    
    // Label
    themeToggleLabel.setText("Theme", dontSendNotification);
    themeToggleLabel.attachToComponent(&themeToggleButton, false);
    // init current theme from saved state
    auroraLookAndFeel.setTheme(themeToggleButton.getToggleState() ? AuroraLookAndFeel::Theme::Dark : AuroraLookAndFeel::Theme::Light);
}

void AuroraAudioProcessorEditor::initChannelSelect() {
    auto* channelMode = dynamic_cast<AudioParameterChoice*>(apvts.getParameter("channelMode"));
    
    addAndMakeVisible(channelButton);
    auto buttonColour = Colours::black.withAlpha(0.5f);
    channelButton.setColour(TextButton::buttonColourId, buttonColour);
    channelButton.setColour(TextButton::buttonOnColourId, buttonColour);
    channelButton.setColour(ComboBox::outlineColourId, buttonColour);
    channelButton.setColour(TextButton::textColourOnId, Colour(232,232,232));
    channelButton.setColour(TextButton::textColourOffId, Colour(232,232,232));
    channelButton.setButtonText(channelMode->getCurrentChoiceName());
    channelButton.onClick = [this, channelMode]() {
        int next = (channelMode->getIndex() + 1) % channelMode->choices.size();
        channelMode->setValueNotifyingHost(channelMode->convertTo0to1(next));
        rgbMeter.setChannelMode(channelMode->choices[next]);
        channelButton.setButtonText(channelMode->choices[next]);
    };
}

void AuroraAudioProcessorEditor::initZoomGroup() {
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

    // Gain Label
    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("Gain", dontSendNotification);
    gainLabel.setJustificationType(Justification::centred);
    
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
    
    // History Label
    addAndMakeVisible(historyLabel);
    historyLabel.attachToComponent(&historySlider, false);
    historyLabel.setText("History", dontSendNotification);
    historyLabel.setJustificationType(Justification::centred);
    
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
    
    // Low Crossover
    addAndMakeVisible(lowCrossoverSlider);
    lowCrossoverSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    lowCrossoverSlider.setSize(paramWidth, paramHeight);
    lowCrossoverSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    lowCrossoverSlider.setTextValueSuffix(" Hz");
    
    // Low crossover label
    addAndMakeVisible(lowCrossoverLabel);
    lowCrossoverLabel.attachToComponent(&lowCrossoverSlider, false);
    lowCrossoverLabel.setText("Low", dontSendNotification);
    lowCrossoverLabel.setJustificationType(Justification::centred);
    
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
    
    // High Crossover label
    addAndMakeVisible(highCrossoverLabel);
    highCrossoverLabel.attachToComponent(&highCrossoverSlider, false);
    highCrossoverLabel.setText("High", dontSendNotification);
    highCrossoverLabel.setJustificationType(Justification::centred);
    
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
    
    // Red Slider
    addAndMakeVisible(redSlider);
    redSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    redSlider.setSize(paramWidth, paramHeight);
    redSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    redSlider.setTextValueSuffix(" %");
    
    // Red Label
    addAndMakeVisible(redLabel);
    redLabel.attachToComponent(&redSlider, false);
    redLabel.setText("Red", dontSendNotification);
    redLabel.setJustificationType(Justification::centred);
    
    // Red Attachment
    redSliderAttachment.reset(new SliderAttachment(apvts, "redWeight", redSlider));
    
    // Red function
    redSlider.onValueChange = [this]() {
        rgbMeter.setColourWeight("red", redSlider.getValue());
    };

    
    // Green Slider
    addAndMakeVisible(greenSlider);
    greenSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    greenSlider.setSize(paramWidth, paramHeight);
    greenSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    greenSlider.setTextValueSuffix(" %");
    
    // Green Label
    addAndMakeVisible(greenLabel);
    greenLabel.attachToComponent(&greenSlider, false);
    greenLabel.setText("Green", dontSendNotification);
    greenLabel.setJustificationType(Justification::centred);
    
    // Green Attachment
    greenSliderAttachment.reset(new SliderAttachment(apvts, "greenWeight", greenSlider));
    
    // Green function
    greenSlider.onValueChange = [this]() {
        rgbMeter.setColourWeight("green", greenSlider.getValue());
    };
    
    
    // Blue Slider
    addAndMakeVisible(blueSlider);
    blueSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    blueSlider.setSize(paramWidth, paramHeight);
    blueSlider.setTextBoxStyle(Slider::TextBoxBelow, false, paramWidth, 24);
    blueSlider.setTextValueSuffix(" %");
    
    
    // Green Label
    addAndMakeVisible(blueLabel);
    blueLabel.attachToComponent(&blueSlider, false);
    blueLabel.setText("Blue", dontSendNotification);
    blueLabel.setJustificationType(Justification::centred);
    
    // Green Attachment
    blueSliderAttachment.reset(new SliderAttachment(apvts, "blueWeight", blueSlider));
    
    // Green function
    blueSlider.onValueChange = [this]() {
        rgbMeter.setColourWeight("blue", blueSlider.getValue());
    };
}
void AuroraAudioProcessorEditor::initControlToggle() {
    
    // add toggle button
    addAndMakeVisible(toggleControlsButton);
    toggleControlsButton.setSize(50, 25);
    toggleControlsButton.setToggleable(true);
    toggleControlsButton.setClickingTogglesState(true);
    
    // Parameter attachment
    toggleControlsAttachment.reset(new ButtonAttachment(apvts, "showControls", toggleControlsButton));
    
    // Show Controls Label
    addAndMakeVisible(toggleControlsLabel);
    toggleControlsLabel.attachToComponent(&toggleControlsButton, false);
    toggleControlsLabel.setSize(75, toggleControlsButton.getHeight());
    toggleControlsLabel.setText("Settings", dontSendNotification);
    toggleControlsLabel.setJustificationType(Justification::centred);
    
    // Hide controls by default
    if (!toggleControlsButton.getToggleState()) {
        setResizeLimits(540, 160, 1280, 720 - groupHeight - margin);
        for (Component* control : controls) {
            control->setVisible(false);
        }
        toggleControlsButton.setButtonText("Show");
    } else {
        setResizeLimits(540, 160 + groupHeight + margin, 1280, 720);
        toggleControlsButton.setButtonText("Hide");
    }
    
    resized(); // force updating bounds
    
    // Click to toggle
    toggleControlsButton.onClick = [this]() {
        if (toggleControlsButton.getToggleState()) {
            showControls();
        } else {
            hideControls();
        }
    };
}

void AuroraAudioProcessorEditor::showControls() {
    setSize(getWidth(), getHeight() + groupHeight + margin);
    setResizeLimits(540, 160 + groupHeight + margin, 1280, 720);
    for (Component* control : controls) {
        control->setVisible(true);
    }
    toggleControlsButton.setButtonText("Hide");
}

void AuroraAudioProcessorEditor::hideControls() {
    setSize(getWidth(), getHeight() - groupHeight - margin);
    setResizeLimits(540, 160, 1280, 720 - groupHeight - margin);
    for (Component* control : controls) {
        control->setVisible(false);
    }
    toggleControlsButton.setButtonText("Show");
}


//==============================================================================
void AuroraAudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(findColour (juce::ResizableWindow::backgroundColourId));
	g.setImageResamplingQuality(Graphics::ResamplingQuality::highResamplingQuality);

	g.setColour(Colour(10, 10, 10));
    
    Image *auroraLogo;
    Image *schematicLogo;
    
    if (auroraLookAndFeel.getTheme() == AuroraLookAndFeel::Theme::Dark) {
        auroraLogo = &logoAuroraDarkTheme;
        schematicLogo = &logoSchematicDarkTheme;
    } else {
        auroraLogo = &logoAuroraLightTheme;
        schematicLogo = &logoSchematicLightTheme;
    }

    if (toggleControlsButton.getToggleState()) {
		// Draw background
        g.fillRoundedRectangle(margin, margin, getWidth() - 2*margin, getHeight() - margin - groupHeight - infoAreaHeight, rgbMeter.getCornerRadius());
        
        // Draw Logos
        g.drawImageWithin(*auroraLogo, margin, getHeight() - infoAreaHeight - groupHeight - margin + 20, 150, infoAreaHeight - 30, RectanglePlacement(RectanglePlacement::Flags::xLeft));
        g.drawImageWithin(*schematicLogo, getWidth() - margin - 160, getHeight() - infoAreaHeight - groupHeight - margin, 160, infoAreaHeight+10, RectanglePlacement());
    } else {
		// Draw background
        g.fillRoundedRectangle(margin, margin, getWidth() - 2*margin, getHeight() - infoAreaHeight, rgbMeter.getCornerRadius());
        
		// Draw Logos
		g.drawImageWithin(*auroraLogo, margin, getHeight() - infoAreaHeight + 20, 150, infoAreaHeight - 30, RectanglePlacement(RectanglePlacement::Flags::xLeft));
        g.drawImageWithin(*schematicLogo, getWidth() - margin - 160, getHeight() - infoAreaHeight, 160, infoAreaHeight+10, RectanglePlacement());
    }
    
}

void AuroraAudioProcessorEditor::resized(){
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
    
    const auto width = getWidth();
    const auto height = getHeight();
    
    updateNotifier.setBounds(getLocalBounds());
    audioProcessor.setEditorSize(width, height);
    
    if (toggleControlsButton.getToggleState()) {
        rgbMeter.setBounds(margin, margin, width - 2*margin, height - margin - groupHeight - infoAreaHeight);
        
        toggleControlsButton.setBounds(width/2 + 5, height - margin - groupHeight - infoAreaHeight/2 + toggleControlsButton.getHeight()/4, toggleControlsButton.getWidth(), toggleControlsButton.getHeight());
    } else {
        rgbMeter.setBounds(margin, margin, width- 2*margin, height - infoAreaHeight);
        
        toggleControlsButton.setBounds(width/2 + 5, height - infoAreaHeight/2 + toggleControlsButton.getHeight()/4, toggleControlsButton.getWidth(), toggleControlsButton.getHeight());
    }
    
    // Theme selection
    themeToggleButton.setBounds(width/2 - 55, toggleControlsButton.getY(), 50, 25);
    
    // Channel selection
    channelButton.setBounds(2*margin, rgbMeter.getY() + rgbMeter.getHeight()/2 - 15, 30, 30);
    
    // Zoom controls
    zoomGroup.setBounds(width - margin+2 - groupWidth, height - margin - groupHeight, groupWidth, groupHeight);
    
    historySlider.setBounds(zoomGroup.getX() + margin, zoomGroup.getY() + labelHeight + margin, paramWidth, paramHeight);
    
    gainSlider.setBounds(historySlider.getX() + paramWidth + margin, historySlider.getY(), paramWidth, paramHeight);
    
    // Crossover controls
    crossoverGroup.setBounds(margin-2, zoomGroup.getY(), groupWidth, groupHeight);
    
    lowCrossoverSlider.setBounds(crossoverGroup.getX() + margin, gainSlider.getY(), paramWidth, paramHeight);
    
    highCrossoverSlider.setBounds(lowCrossoverSlider.getX() + paramWidth + margin, lowCrossoverSlider.getY(), paramWidth, paramHeight);
    
    // Colour controls
    colourGroup.setBounds(width/2 - 1.5*paramWidth - 2*margin, crossoverGroup.getY(), 3*paramWidth + 4*margin, groupHeight);
    
    redSlider.setBounds(colourGroup.getX() + margin, gainSlider.getY(), paramWidth, paramHeight);
    greenSlider.setBounds(redSlider.getX() + paramWidth + margin, redSlider.getY(), paramWidth, paramHeight);
    blueSlider.setBounds(greenSlider.getX() + paramWidth + margin, greenSlider.getY(), paramWidth, paramHeight);
    
}
