/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

CompanyLogo::CompanyLogo ()
{
    logo = Drawable::createFromImageData (BinaryData::SchematicSoundLogo_png, BinaryData::SchematicSoundLogo_pngSize);

    #if ANIMATE_COMPANY_LOGO
        jitterX.reset (15);
        jitterY.reset (15);
        startTimerHz (30);
    #endif
}

void CompanyLogo::timerCallback ()
{
    // this is just a simple example of how to animate the logo... this particular code makes the logo shiver
    const auto jitterRange = 0.1f;
    jitterX.setTargetValue (jmap (random.nextFloat(), 0.f, 1.f, -jitterRange, jitterRange));
    jitterY.setTargetValue (jmap (random.nextFloat(), 0.f, 1.f, -jitterRange, jitterRange));
    repaint ();
}

void CompanyLogo::paint (Graphics& g)
{
    const auto width = getWidth ();
    const auto height = getHeight ();
    auto area = getLocalBounds().toFloat().reduced (height * 0.1f);
    
    #if ANIMATE_COMPANY_LOGO
        const auto currentJitterX = jitterX.getNextValue ();
        const auto currentJitterY = jitterY.getNextValue ();
        area = area.translated (width * currentJitterX, height * currentJitterY);
    #endif
    
    if (logo != nullptr)
        logo->drawWithin (g, area, RectanglePlacement::centred, 1.0f);
}

//==============================================================================
AuroraAudioProcessorEditor::AuroraAudioProcessorEditor(AuroraAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
    
    LookAndFeel::setDefaultLookAndFeel(&customLookAndFeel);
    
    // add RGB Meter
    addAndMakeVisible(rgbMeter);
    
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
    
    initControlToggle();
    
    // Moonbase Activation UI
    activationUI->addListener(this);
    if (activationUI)
    {
        // There are a max of 2 lines of text on the welcome screen, define them here
        activationUI->setWelcomePageText ("Audio will mute occasionally while unactivated", "");

        // Set the logo inside the spinner (when waiting for web responses)
        activationUI->setSpinnerLogo (Drawable::createFromImageData (BinaryData::AuroraLogoLightMode_png, BinaryData::AuroraLogoLightMode_pngSize));

        // Scale the spinner logo as required for your asset if needed. See Submodules/moonbase_JUCEClient/Assets/Source/SVG/OverlayAssets for ideal assets.
        // activationUI->setSpinnerLogoScale (0.5f);
        
        // Set the company logo, this is the logo that is displayed on the welcome screen and the activated info screen
        activationUI->setCompanyLogo (std::make_unique<CompanyLogo> ());
        // Scale the company logo as required for your asset if needed.
        // activationUI->setCompanyLogoScale ((0.25f));
    }
    
    // Check for updates
    addChildComponent(updateNotifier);
    if (!audioProcessor.isUpdateDismissed()) {
        updateNotifier.checkForUpdates();
        if (updateNotifier.isUpdateAvailable()) {
            updateNotifier.setVisible(true);
        }
    }

}

AuroraAudioProcessorEditor::~AuroraAudioProcessorEditor()
{
    if (activationUI != nullptr) {
        activationUI->removeListener(this);
    }
}

void AuroraAudioProcessorEditor::onActivationUiVisibilityChanged (const ActivationUI::Visibility& visibility) {
    jassert (activationUI != nullptr);
    if (activationUI == nullptr)
        return;

    const auto width = getWidth();
    const auto height = getHeight();

    if (visibility.isVisible) {
        auto requiresResize = false;
        if (requiresRestore) {
            setSize(restoreWidth, restoreHeight);
            requiresRestore = false;
            return;
        }
        if (width < 600 || height < 450) {
            restoreWidth = width;
            restoreHeight = height;
            requiresResize = true;
        }
        if (requiresResize) {
            auto newWidth = std::max(width, 600);
            auto newHeight = std::max(height, 450);
            setSize(newWidth, newHeight);
            requiresRestore = true;
        }
    }
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
    };
}
void AuroraAudioProcessorEditor::initControlToggle() {
    
    // add toggle button
    addAndMakeVisible(toggleControlsButton);
    toggleControlsButton.setSize(21, 21);
    toggleControlsButton.setColour(ToggleButton::tickColourId, Colour(32,32,32));
    toggleControlsButton.setColour(ToggleButton::tickDisabledColourId, Colour(32,32,32));
    
    // Parameter attachment
    toggleControlsAttachment.reset(new ButtonAttachment(apvts, "showControls", toggleControlsButton));
    
    // Show Controls Label
    addAndMakeVisible(toggleControlsLabel);
    toggleControlsLabel.setSize(75, toggleControlsButton.getHeight());
    toggleControlsLabel.setText("Show Controls", dontSendNotification);
    toggleControlsLabel.setJustificationType(Justification::centred);
    toggleControlsLabel.setColour(Label::ColourIds::textColourId, Colour(32,32,32));
    
    // Hide controls by default
    if (!toggleControlsButton.getToggleState()) {
        setResizeLimits(540, 160, 1280, 720 - groupHeight - margin);
        for (Component* control : controls) {
            control->setVisible(false);
        }
    } else {
        setResizeLimits(540, 160 + groupHeight + margin, 1280, 720);
    }
    
    resized(); // force updating bounds
    
    // Click to toggle
    toggleControlsButton.onClick = [this]() {
        if (toggleControlsButton.getToggleState()) {
            showControls();
        } else {
            hideControls();
        }
        repaint();
    };
}

void AuroraAudioProcessorEditor::showControls() {
    setSize(getWidth(), getHeight() + groupHeight + margin);
    setResizeLimits(540, 160 + groupHeight + margin, 1280, 720);
    for (Component* control : controls) {
        control->setVisible(true);
    }
}

void AuroraAudioProcessorEditor::hideControls() {
    setSize(getWidth(), getHeight() - groupHeight - margin);
    setResizeLimits(540, 160, 1280, 720 - groupHeight - margin);
    for (Component* control : controls) {
        control->setVisible(false);
    }
}


//==============================================================================
void AuroraAudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll(Colour(32,32,32));
    g.fillAll(Colour(232, 232, 232));

    if (toggleControlsButton.getToggleState()) {
        g.fillRoundedRectangle(margin, margin, getWidth() - 2*margin, getHeight() - margin - groupHeight - infoAreaHeight, rgbMeter.getCornerRadius());
        
        g.drawImageAt(logoAurora, margin, getHeight() - margin - groupHeight - infoAreaHeight/2 - logoAurora.getHeight()/2);
        g.drawImageAt(logoSchematic, getWidth() - margin - logoSchematic.getWidth(), getHeight() - margin - groupHeight - infoAreaHeight/2 - logoSchematic.getHeight()/3);
    } else {
        g.fillRoundedRectangle(margin, margin, getWidth() - 2*margin, getHeight() - infoAreaHeight, rgbMeter.getCornerRadius());
        
        g.drawImageAt(logoAurora, margin, getHeight() - infoAreaHeight/2 - logoAurora.getHeight()/2);
        g.drawImageAt(logoSchematic, getWidth() - margin - logoSchematic.getWidth(), getHeight() - infoAreaHeight/2 - logoSchematic.getHeight()/3);
    }
    
}

void AuroraAudioProcessorEditor::resized(){
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
    
    MOONBASE_RESIZE_ACTIVATION_UI
    
    const auto width = getWidth();
    const auto height = getHeight();
    
    updateNotifier.setBounds(getLocalBounds());

//    auto labelHeight = gainLabel.getHeight();
    audioProcessor.setEditorSize(width, height);
    
    
//    darkModeButton.setBounds(margin, margin, paramWidth, paramHeight);
    if (toggleControlsButton.getToggleState()) {
        rgbMeter.setBounds(margin, margin, width - 2*margin, height - margin - groupHeight - infoAreaHeight);
        
        toggleControlsButton.setBounds(width/2 - toggleControlsButton.getWidth()/2, height - margin -  groupHeight - infoAreaHeight/2 + toggleControlsButton.getHeight()/4, toggleControlsButton.getWidth(), toggleControlsButton.getHeight());
        toggleControlsLabel.setBounds(width/2 - toggleControlsLabel.getWidth()/2, toggleControlsButton.getY() - toggleControlsLabel.getHeight(), toggleControlsLabel.getWidth(), toggleControlsLabel.getHeight());
    } else {
        rgbMeter.setBounds(margin, margin, width- 2*margin, height - infoAreaHeight);
        
        toggleControlsButton.setBounds(width/2 - toggleControlsButton.getWidth()/2, height - infoAreaHeight/2 + toggleControlsButton.getHeight()/4, toggleControlsButton.getWidth(), toggleControlsButton.getHeight());
        toggleControlsLabel.setBounds(width/2 - toggleControlsLabel.getWidth()/2, toggleControlsButton.getY() - toggleControlsLabel.getHeight(), toggleControlsLabel.getWidth(), toggleControlsLabel.getHeight());
    }
    
    
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
