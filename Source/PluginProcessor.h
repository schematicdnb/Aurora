/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 */

namespace juce
{
    class RGBMeter : public AudioVisualiserComponent
    {

    public:
        Array<Colour> colHistory;

        RGBMeter() : AudioVisualiserComponent(1)
        {
            // while (colHistory.size() < 512)
            // {
            //     colHistory.add(Colour(0, 0, 0));
            // }
            colHistory.resize(512);
            setBufferSize(512);

            // setSamplesPerBlock(256);
            // setColours(Colours::black, Colour(255, 0, 0));
            // setRepaintRate(30);
        }

        void setNextSampleColour(int nextSample, Colour c)
        {
            colHistory.set(nextSample, c);
        }

        void paintChannel(Graphics &graphic, Rectangle<float> area,
                          const Range<float> *levels, int numLevels, int nextSample) override
        {

            //            setNextSampleColour(nextSample, Colour(
            //                                                Random::getSystemRandom().nextInt(256),
            //                                                Random::getSystemRandom().nextInt(256),
            //                                                Random::getSystemRandom().nextInt(256)));

            setNextSampleColour(nextSample, Colour(
                                                255,
                                                0,
                                                0));

            for (int i = 0; i < numLevels; ++i)
            {
                // get start and end points of segment
                auto start = -(levels[(nextSample + i) % numLevels].getStart());
                auto end = -(levels[(nextSample + i) % numLevels].getEnd());

                // draw segment line

                Path path;
                //                if (i == 0) {
                path.startNewSubPath((float)i, start);
                //                } else {
                path.lineTo((float)i, end);
                //                }

                //                path.lineTo((float)i+1, end);
                path.closeSubPath();

                // colour the segment
                graphic.setColour(colHistory[(nextSample + i) % numLevels]);

                graphic.strokePath(path, PathStrokeType(1.0f), AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(), 0.0f, 1.0f, area.getX(), area.getBottom(), (float)numLevels, -1.0f, area.getRight(), area.getY()));
            }
        }
    };
}

class RGBMeterAudioProcessor : public juce::AudioProcessor
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
    int r, g, b;

//    using APVTS = juce::AudioProcessorValueTreeState;
//    static APVTS::ParameterLayout createParameterLayout();
//
//    APVTS apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LP, HP;
    juce::AudioParameterFloat* lowCrossover {nullptr};
    juce::AudioParameterFloat* midLowCrossover {nullptr};
    juce::AudioParameterFloat* midHighCrossover {nullptr};
    juce::AudioParameterFloat* highCrossover {nullptr};
    juce::AudioParameterBool* bypassLow {nullptr};
    juce::AudioParameterBool* bypassMid {nullptr};
    juce::AudioParameterBool* bypassHigh {nullptr};
    std::array<juce::AudioBuffer<float>, 2> filterBuffers;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RGBMeterAudioProcessor)
};
