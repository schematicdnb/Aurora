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

    addParameter(lowCrossover = new juce::AudioParameterFloat(juce::ParameterID{"lowCrossover", 1}, "Low Crossover", 20.0f, 20000.0f, 200.0f));
    addParameter(midLowCrossover = new juce::AudioParameterFloat(juce::ParameterID{"midLowCrossover", 1}, "Mid Low Crossover", 20.0f, 20000.0f, 200.0f));
    addParameter(midHighCrossover = new juce::AudioParameterFloat(juce::ParameterID{"midHighCrossover", 1}, "Mid High Crossover", 2000.0f, 20000.0f, 5000.0f));
    addParameter(highCrossover = new juce::AudioParameterFloat(juce::ParameterID{"highCrossover", 1}, "High Crossover", 2000.0f, 20000.0f, 5000.0f));
    addParameter(bypassLow = new juce::AudioParameterBool(juce::ParameterID{"bypassLow", 1}, "Bypass Low", false));
    addParameter(bypassMid = new juce::AudioParameterBool(juce::ParameterID{"bypassMid", 1}, "Bypass Mid", false));
    addParameter(bypassHigh = new juce::AudioParameterBool(juce::ParameterID{"bypassHigh", 1}, "Bypass High", false));
}

RGBMeterAudioProcessor::~RGBMeterAudioProcessor()
{
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

    //    juce::dsp::ProcessSpec spec;
    //    spec.sampleRate = sampleRate;
    //    spec.maximumBlockSize = samplesPerBlock;
    //    spec.numChannels = getTotalNumInputChannels();

    //    lowFilter.prepare(spec);
    //    midFilter.prepare(spec);
    //    highFilter.prepare(spec);
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

        // Split the signal into three frequency bands: low, mid, high
        //        juce::dsp::LinkwitzRileyFilter<float> lowPassFilter;
        //        juce::dsp::LinkwitzRileyFilter<float> bandPassFilter;
        //        juce::dsp::LinkwitzRileyFilter<float> highPassFilter;

        // Set up the filters
        //        lowPassFilter.setCoefficients(juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(200.0f, getSampleRate(), 2)[0]);
        //        bandPassFilter.setCoefficients(juce::dsp::FilterDesign<float>::designIIRBandpassHighOrderButterworthMethod(200.0f, 2000.0f, getSampleRate(), 2)[0]);
        //        highPassFilter.setCoefficients(juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(2000.0f, getSampleRate(), 2)[0]);

        // Apply the filters to the buffer
        //        juce::AudioBuffer<float> lowBuffer(buffer);
        //        juce::AudioBuffer<float> midBuffer(buffer);
        //        juce::AudioBuffer<float> highBuffer(buffer);

        //        lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(lowBuffer));
        //        bandPassFilter.process(juce::dsp::ProcessContextReplacing<float>(midBuffer));
        //        highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(highBuffer));

        // Combine the processed buffers back into the original buffer
        //        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        //        {
        //            buffer.setSample(channel, sample,
        //                             lowBuffer.getSample(channel, sample) +
        //                                 midBuffer.getSample(channel, sample) +
        //                                 highBuffer.getSample(channel, sample));
        //        }

        rgbMeter.pushBuffer(buffer);
    }
}

//==============================================================================
bool RGBMeterAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *RGBMeterAudioProcessor::createEditor()
{
    return new RGBMeterAudioProcessorEditor(*this);
}

//==============================================================================
void RGBMeterAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    //    juce::MemoryOutputStream mos(destData, true);
    //    apvts.state.writeToStream(mos);
}

void RGBMeterAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    //    if (tree.isValid()) {
    //        apvts.replaceState(tree);
    //    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new RGBMeterAudioProcessor();
}

// juce::AudioProcessorValueTreeState::ParameterLayout RGBMeterAudioProcessor::createParameterLayout()
//{
//
//     using namespace juce;
//     APVTS::ParameterLayout layout;
//
//     layout.add(std::make_unique<AudioParameterBool>("bypassLow", "Bypass", false));
//     layout.add(std::make_unique<AudioParameterBool>("bypassMid", "Bypass", false));
//     layout.add(std::make_unique<AudioParameterBool>("bypassHigh", "Bypass", false));
//
//     layout.add(std::make_unique<AudioParameterFloat>("lowCrossover", "Low Crossover", NormalisableRange<float>(20, 200000, 1, 1), 200));
//
//
//
//     return layout;
// }
