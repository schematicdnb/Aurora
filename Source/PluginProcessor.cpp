/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RGBMeterAudioProcessor::RGBMeterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      rgbMeter()
#endif
{
    // initialize crossovers params
    lowCrossover = dynamic_cast<juce::AudioParameterFloat *>(apvts.getParameter("lowCrossover"));
    jassert(lowCrossover != nullptr);

    highCrossover = dynamic_cast<juce::AudioParameterFloat *>(apvts.getParameter("highCrossover"));
    jassert(highCrossover != nullptr);

    // intialize enable/bypass params
    enableLow = dynamic_cast<juce::AudioParameterBool *>(apvts.getParameter("enableLow"));
    jassert(enableLow != nullptr);
    enableMid = dynamic_cast<juce::AudioParameterBool *>(apvts.getParameter("enableMid"));
    jassert(enableMid != nullptr);
    enableHigh = dynamic_cast<juce::AudioParameterBool *>(apvts.getParameter("enableHigh"));
    jassert(enableHigh != nullptr);

    // initialize parameter listeners
    apvts.addParameterListener("lowCrossover", this);
    apvts.addParameterListener("highCrossover", this);

    // intialize filter types
    LP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    midLP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    midAP.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    midHP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    // Initialize crossover points
    LP.setCutoffFrequency(lowCrossover->get());
    midHP.setCutoffFrequency(lowCrossover->get());
    midAP.setCutoffFrequency(highCrossover->get());
    midLP.setCutoffFrequency(highCrossover->get());
    HP.setCutoffFrequency(highCrossover->get());
}

RGBMeterAudioProcessor::~RGBMeterAudioProcessor()
{
    apvts.removeParameterListener("lowCrossover", this);
    apvts.removeParameterListener("highCrossover", this);
}

//==============================================================================
const juce::String RGBMeterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RGBMeterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RGBMeterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool RGBMeterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double RGBMeterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RGBMeterAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int RGBMeterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RGBMeterAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String RGBMeterAudioProcessor::getProgramName(int index)
{
    return {};
}

void RGBMeterAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void RGBMeterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    LP.prepare(spec);
    midLP.prepare(spec);
    midAP.prepare(spec);
    midHP.prepare(spec);
    HP.prepare(spec);
}

void RGBMeterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RGBMeterAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void RGBMeterAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
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

    // instantiate band buffers
    juce::AudioBuffer<float> lowBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> midBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> highBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    lowBuffer = buffer;
    midBuffer = buffer;

    // instantiate blocks
    auto lowBlock = juce::dsp::AudioBlock<float>(lowBuffer);
    auto midBlock = juce::dsp::AudioBlock<float>(midBuffer);
    auto highBlock = juce::dsp::AudioBlock<float>(highBuffer);

    // instantiate processing contexts
    auto lowContext = juce::dsp::ProcessContextReplacing<float>(lowBlock);
    auto midContext = juce::dsp::ProcessContextReplacing<float>(midBlock);
    auto highContext = juce::dsp::ProcessContextReplacing<float>(highBlock);

    // process filters
    LP.process(lowContext);
    midAP.process(lowContext);

    midHP.process(midContext);
    highBuffer = midBuffer;
    midLP.process(midContext);

    HP.process(highContext);

    //    buffer.clear();
    //
    //    // Combine the processed buffers back into the original buffer
    //    for (int i = 0; i < 3; i++) {
    //        auto band = std::vector<juce::AudioBuffer<float>>{lowBuffer, midBuffer, highBuffer}[i];
    //        auto enabled = std::vector<juce::AudioParameterBool*>{enableLow, enableMid, enableHigh}[i];
    //        if (enabled->get()) {
    //            for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
    //                buffer.addFrom(channel, 0, band, channel, 0, band.getNumSamples());
    //            }
    //        }
    //
    //    }

    rgbMeter.setBufferColour(freqToColour(lowBuffer, midBuffer, highBuffer));

    rgbMeter.pushBuffer(buffer);
}

juce::Colour RGBMeterAudioProcessor::freqToColour(juce::AudioBuffer<float> &lowBuffer, juce::AudioBuffer<float> &midBuffer, juce::AudioBuffer<float> &highBuffer)
{
    // get RMS for each frequency band
    auto lowRMS = getRMS(lowBuffer);
    auto midRMS = getRMS(midBuffer);
    auto highRMS = getRMS(highBuffer);

    auto fullRMS = lowRMS + midRMS + highRMS;

    // normalize
     if (fullRMS > 0.0f)
     {
         lowRMS /= fullRMS;
         midRMS /= fullRMS;
         highRMS /= fullRMS;
     }

    // Only allow colour band if button enabled
    for (int i = 0; i < 3; i++)
    {
        auto enabled = std::vector<juce::AudioParameterBool *>{enableLow, enableMid, enableHigh}[i];
        if (!enabled->get())
        {
            switch (i)
            {
            case 0:
                lowRMS = 0;
                break;
            case 1:
                midRMS = 0;
                break;
            case 2:
                highRMS = 0;
                break;
            }
        }
    }

    juce::Colour colour = juce::Colour(std::floor(lowRMS * 255), std::floor(midRMS * 255), std::floor(highRMS * 255));

    return colour;
}

// float RGBMeterAudioProcessor::getPower(juce::AudioBuffer<float> &buffer, int channel)
float RGBMeterAudioProcessor::getRMS(juce::AudioBuffer<float> &buffer)
{
    auto power = 0.0f;
    auto numChannels = buffer.getNumChannels();
    for (int channel = 0; channel < numChannels; channel++)
    {
        power += buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
    }
    return power / numChannels;
}

//==============================================================================
bool RGBMeterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *RGBMeterAudioProcessor::createEditor()
{
    //        return new juce::GenericAudioProcessorEditor(*this);
    return new RGBMeterAudioProcessorEditor(*this);
}

//==============================================================================
void RGBMeterAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream stream(destData, true);
    apvts.state.writeToStream(stream);
}

void RGBMeterAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout RGBMeterAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout params;

    auto lowCrossoverRange = std::make_unique<juce::NormalisableRange<float>>(20.0f, 999.0f, 1.0f, 1.0f);
    auto highCrossoverRange = std::make_unique<juce::NormalisableRange<float>>(1000.0f, 20000.0f, 1.0f, 1.0f);

    params.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowCrossover", 1),
        "Low Crossover",
        *lowCrossoverRange,
        150.0f));

    params.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("highCrossover", 1),
        "High Crossover",
        *highCrossoverRange,
        2000.0f));

    params.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("enableLow", 1), "Enable Low", true));
    params.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("enableMid", 1), "Enable Mid", true));
    params.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("enableHigh", 1), "Enable High", true));

    return params;
}

void RGBMeterAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{

    if (parameterID == "lowCrossover")
    {
        LP.setCutoffFrequency(lowCrossover->get());
        midHP.setCutoffFrequency(lowCrossover->get());
    }
    else if (parameterID == "highCrossover")
    {
        midAP.setCutoffFrequency(highCrossover->get());
        midLP.setCutoffFrequency(highCrossover->get());
        HP.setCutoffFrequency(highCrossover->get());
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new RGBMeterAudioProcessor();
}
