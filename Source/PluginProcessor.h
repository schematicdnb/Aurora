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
        RGBMeter() : AudioVisualiserComponent(1)
        {
            while (colHistory.size() < 512)
            {
                colHistory.add(Colour(0, 0, 0));
            }

            setBufferSize(512);
            setSamplesPerBlock(256);
            setColours(Colours::black, Colour(255, 0, 0));
            setRepaintRate(30);
        }

        // int r = 255, g = 0, b = 0;
        int r, g, b;
        Array<Colour> colHistory;

        void paintChannel(Graphics &graphic, Rectangle<float> area,
                          const Range<float> *levels, int numLevels, int nextSample) override
        {

            // if (b == 255)
            // {
            //     r = 255;
            //     g = 0;
            //     b = 0;
            // }
            // if (g < 255)
            // {
            //     g++;
            // }
            // //            if (r == 255) {g++;}
            // if (g == 255)
            // {
            //     b++;
            // }
            r = 255;
            g = Random::getSystemRandom().nextInt(256);
            b = Random::getSystemRandom().nextInt(256);
            //            g = 255;
            //            b = 255;

            Colour c = Colour(r, g, b);
            colHistory.set(nextSample, c);

            for (int i = 0; i < numLevels; ++i)
            {
                // get start and end points of segment
                auto start = -(levels[(nextSample + i) % numLevels].getStart());
                auto end = -(levels[(nextSample + i) % numLevels].getEnd());

                // draw segment line
                Path path;
                path.startNewSubPath((float)i, start);
                path.lineTo((float)i, end);
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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RGBMeterAudioProcessor)
};
