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
        Colour bufferColour;

        RGBMeter() : AudioVisualiserComponent(1)
        {
            // while (colHistory.size() < 512)
            // {
            //     colHistory.add(Colour(0, 0, 0));
            // }
            colHistory.resize(512);
            setBufferSize(512);
            bufferColour = Colour(255, 255, 255);

            // setSamplesPerBlock(256);
            // setColours(Colours::black, Colour(255, 0, 0));
            // setRepaintRate(30);
        }

        void setBufferColour(Colour c)
        {
            bufferColour = c;
        }

        void setNextSampleColour(int nextSample, Colour c)
        {
            colHistory.set(nextSample, c);
        }

        //         paint channel as freq-based colour for whole waveform
        void paintChannel(Graphics &g, Rectangle<float> area,
                          const Range<float> *levels, int numLevels, int nextSample)
        {
            Path p;
            getChannelAsPath(p, levels, numLevels, nextSample);

            g.setColour(bufferColour);

            g.fillPath(p, AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(),
                                                            0.0f, 1.0f, area.getX(), area.getBottom(),
                                                            (float)numLevels, -1.0f, area.getRight(), area.getY()));
        }

        //        // custom paintchannel backup
        //        void paintChannel(Graphics &graphic, Rectangle<float> area,
        //                          const Range<float> *levels, int numLevels, int nextSample) override
        //        {
        //
        //            setNextSampleColour(nextSample, bufferColour);
        //
        //            // setNextSampleColour(nextSample, Colour(
        //            //                                     Random::getSystemRandom().nextInt(256),
        //            //                                     Random::getSystemRandom().nextInt(256),
        //            //                                     Random::getSystemRandom().nextInt(256)));
        //
        //            for (int i = 0; i < numLevels; ++i)
        //            {
        //                // get start and end points of segment
        //                auto start = -(levels[(nextSample + i) % numLevels].getStart());
        //                auto end = -(levels[(nextSample + i) % numLevels].getEnd());
        //
        //                // draw segment line
        //                Path path;
        //                path.startNewSubPath((float)i, start);
        //                path.lineTo((float)i + 1, end);
        //                path.closeSubPath();
        //
        //                // colour the segment
        //                graphic.setColour(colHistory[(nextSample + i) % numLevels]);
        //                // graphic.setColour(colour);
        //
        //                graphic.strokePath(path, PathStrokeType(1.0f), AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(), 0.0f, 1.0f, area.getX(), area.getBottom(), (float)numLevels, -1.0f, area.getRight(), area.getY()));
        //            }
        //        }
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
    float getPower(juce::AudioBuffer<float> &buffer);

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{*this, nullptr, "Parameters", createParameterLayout()};

    

private:
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LP, midLP, midAP, midHP, HP;

    juce::AudioParameterFloat *lowCrossover{nullptr};
    juce::AudioParameterFloat *highCrossover{nullptr};
    
    juce::AudioParameterBool *enableLow{nullptr};
    juce::AudioParameterBool *enableMid{nullptr};
    juce::AudioParameterBool *enableHigh{nullptr};
    
    juce::Colour colour;
    // std::array<juce::AudioBuffer<float>, 2> filterBuffers;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RGBMeterAudioProcessor)
};
