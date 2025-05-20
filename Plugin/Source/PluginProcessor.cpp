/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AuroraAudioProcessor::AuroraAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         )
#endif
{
    initDSP();
}

AuroraAudioProcessor::~AuroraAudioProcessor()
{
}

void AuroraAudioProcessor::initDSP() {
    // set History Length
    rgbMeter.setHistoryLength(static_cast<int>(*apvts.getRawParameterValue("historyLength")));
    
    // Set gain
    rgbMeter.setGain(*apvts.getRawParameterValue("gain"));
    
    // Set crossovers
    rgbMeter.setLowCrossover(static_cast<float>(*apvts.getRawParameterValue("lowCrossover")));
    rgbMeter.setHighCrossover(static_cast<float>(*apvts.getRawParameterValue("highCrossover")));
    
    // Set colour mixer
    rgbMeter.setColourWeight("red", static_cast<float>(*apvts.getRawParameterValue("redWeight")));
    rgbMeter.setColourWeight("green", static_cast<float>(*apvts.getRawParameterValue("greenWeight")));
    rgbMeter.setColourWeight("blue", static_cast<float>(*apvts.getRawParameterValue("blueWeight")));
    
}

//==============================================================================
const juce::String AuroraAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AuroraAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AuroraAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AuroraAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AuroraAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AuroraAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int AuroraAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AuroraAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AuroraAudioProcessor::getProgramName(int index)
{
    return {};
}

void AuroraAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void AuroraAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    MOONBASE_PREPARE_TO_PLAY (sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    rgbMeter.prepare(spec);
}

void AuroraAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AuroraAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where y ou check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void AuroraAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //=====================================================

    rgbMeter.pushSamples(buffer);
    
    MOONBASE_PROCESS (buffer);
}

//==============================================================================
bool AuroraAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AuroraAudioProcessor::createEditor()
{
    //        return new juce::GenericAudioProcessorEditor(*this);
    return new AuroraAudioProcessorEditor(*this);
}

//==============================================================================
void AuroraAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream stream(destData, true);
    apvts.state.writeToStream(stream);
}

void AuroraAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
    initDSP();
}

juce::AudioProcessorValueTreeState::ParameterLayout AuroraAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout params;
    
    // History
    params.add(std::make_unique<AudioParameterInt>(ParameterID("historyLength", 1), "History Length", 1, 20, 4));

    // Gain
    auto gainRange = std::make_unique<NormalisableRange<float>>(-24.0f, 24.0f, 0.1f);
    params.add(std::make_unique<AudioParameterFloat>(ParameterID("gain", 1), "Gain", *gainRange, 0.0f));
    
    // Low Crossover
    auto lowCrossoverRange = std::make_unique<NormalisableRange<float>>(20.0f, 20000.0f, 1.0f, 0.2);
    params.add(std::make_unique<AudioParameterFloat>(ParameterID("lowCrossover", 1), "Low Crossover", *lowCrossoverRange, 250.0));
    
    // High Crossover
    auto highCrossoverRange = std::make_unique<NormalisableRange<float>>(20.0f, 20000.0f, 1.0f, 3.0);
    params.add(std::make_unique<AudioParameterFloat>(ParameterID("highCrossover", 1), "High Crossover", *lowCrossoverRange, 2000.0f));
    
    // Colour weights
    auto colourWeightRange = std::make_unique<NormalisableRange<float>>(0.0f, 2.0f, 0.01f);
    auto percent = AudioParameterFloatAttributes()
        .withStringFromValueFunction([](float value, int) {
        return String(value * 100);
        })
        .withValueFromStringFunction([](const String& text) {
            return text.getFloatValue() / 100;
        });
    params.add(std::make_unique<AudioParameterFloat>(ParameterID("redWeight", 1), "Red Weight", *colourWeightRange, 1.0f, percent));
    params.add(std::make_unique<AudioParameterFloat>(ParameterID("greenWeight", 1), "Green Weight", *colourWeightRange, 1.0f, percent));
    params.add(std::make_unique<AudioParameterFloat>(ParameterID("blueWeight", 1), "Blue Weight", *colourWeightRange, 1.0f, percent));
    
    params.add(std::make_unique<AudioParameterBool>(ParameterID("darkMode", 1), "Dark Mode", false));
    
    params.add(std::make_unique<AudioParameterBool>(ParameterID("showControls", 1), "Show Controls", false));
    
    return params;
}

int AuroraAudioProcessor::getEditorWidth() {
    auto size = apvts.state.getOrCreateChildWithName("size", nullptr);
    return size.getProperty("width", 540);
}

int AuroraAudioProcessor::getEditorHeight() {
    auto size = apvts.state.getOrCreateChildWithName("size", nullptr);
    return size.getProperty("height", 160);
}

void AuroraAudioProcessor::setEditorSize(int width, int height) {
    auto size = apvts.state.getOrCreateChildWithName("size", nullptr);
    size.setProperty("width", width, nullptr);
    size.setProperty("height", height, nullptr);
}

void AuroraAudioProcessor::dismissUpates() {
    updatesDismissed = true;
}

bool AuroraAudioProcessor::isUpdatesDismissed() {
    if (updatesDismissed) {return true;}
    return false;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AuroraAudioProcessor();
}
