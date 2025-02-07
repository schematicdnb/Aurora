/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RGBMeter.h"

//==============================================================================
/**
 */

class RGBMeterAudioProcessor : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    RGBMeterAudioProcessor();
    ~RGBMeterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::RGBMeter rgbMeter;
    juce::AudioVisualiserComponent avc{1};
    float getRMS(juce::AudioBuffer<float> &buffer);
    juce::Colour freqToColour(juce::AudioBuffer<float> &lowBuffer, juce::AudioBuffer<float> &midBuffer, juce::AudioBuffer<float> &highBuffer);

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{*this, nullptr, "Parameters", createParameterLayout()};
    
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    

private:
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LP, midLP, midAP, midHP, HP;

    juce::AudioParameterFloat *lowCrossover{nullptr};
    juce::AudioParameterFloat *highCrossover{nullptr};
    
    juce::AudioParameterBool *enableLow{nullptr};
    juce::AudioParameterBool *enableMid{nullptr};
    juce::AudioParameterBool *enableHigh{nullptr};
    
    // juce::Colour colour;
    // std::array<juce::AudioBuffer<float>, 2> filterBuffers;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RGBMeterAudioProcessor)
};
