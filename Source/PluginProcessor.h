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
            colHistory = {};

            setBufferSize(512);
            setSamplesPerBlock(256);
            setColours(Colours::black, Colour(255, 0, 0));
            setRepaintRate(30);
        }

        Array<Colour> colHistory;

//        void paintChannel(Graphics &graphic, Rectangle<float> area,
//                          const Range<float> *segments, int numSegments, int nextSample) override
//        {
//            int r, g, b;
//            r = 255;
//            g = Random::getSystemRandom().nextInt(256);
//            b = Random::getSystemRandom().nextInt(256);
//
//            Colour c = Colour(r, g, b);
//            if (colHistory.size() >= numSegments)
//            {
//                colHistory.remove(0);
//            }
//            colHistory.add(c);
//
//            for (int i = 0; i < numSegments; i++)
//            {
//                Path segment;
//                segment.startNewSubPath((float)i, -(segments[(nextSample + i) % numSegments].getEnd()));
//                segment.lineTo((float)i, -(segments[(nextSample + i) % numSegments].getStart()));
//
////                graphic.setColour(colHistory[i]);
//                graphic.setColour(c);
//                // Draw the path for the current segment
//                graphic.strokePath(segment, PathStrokeType(1.0f), AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(),
//                                                                                                 0.0f, 1.0f, area.getX(), area.getBottom(),
//                                                                                                 (float)numSegments, -1.0f, area.getRight(), area.getY()));
//                    
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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RGBMeterAudioProcessor)
};
