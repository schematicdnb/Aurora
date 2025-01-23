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
//            while (colHistory.size() < 512)
//            {
//                colHistory.add(Colour(0, 0, 0));
//            }

            setBufferSize(512);
            setSamplesPerBlock(256);
            setColours(Colours::black, Colour(255, 0, 0));
            setRepaintRate(30);
        }

        Array<Colour> colHistory;

        // void colourPath(Path &path, const Range<float> *levels,
        //                 int numLevels, int nextSample)
        // {
        //     path.preallocateSpace(4 * numLevels + 8);

        //     for (int i = 0; i < numLevels; ++i)
        //     {
        //         auto level = -(levels[(nextSample + i) % numLevels].getEnd());

        //         if (i == 0)
        //             path.startNewSubPath(0.0f, level);
        //         else
        //             path.lineTo((float)i, level);

        //     }

        //     for (int i = numLevels; --i >= 0;)
        //         path.lineTo((float)i, -(levels[(nextSample + i) % numLevels].getStart()));

        //     path.closeSubPath();
        // }

        void paintChannel(Graphics &graphic, Rectangle<float> area,
                          const Range<float> *levels, int numLevels, int nextSample) override
        {
            int r, g, b;
            r = 255;
            g = Random::getSystemRandom().nextInt(256);
            b = Random::getSystemRandom().nextInt(256);
            //            g = 255;
            //            b = 255;

            Colour c = Colour(r, g, b);
            if (colHistory.size() > numLevels) {
                colHistory.remove((nextSample + 1) % numLevels);
            }
            colHistory.add(c);
            
//            std::cout << numLevels << std::endl;

            // Path path;
            // path.preallocateSpace(4 * numLevels + 8);

            // for (int i = 0; i < numLevels; ++i)
            // {
            //     auto level = -(levels[(nextSample + i) % numLevels].getEnd());

            //     if (i == 0)
            //         path.startNewSubPath(0.0f, level);
            //     else
            //         path.lineTo((float)i, level);
            // }

            // for (int i = numLevels; --i >= 0;)
            //     path.lineTo((float)i, -(levels[(nextSample + i) % numLevels].getStart()));

            // path.closeSubPath();

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

            // for (int i = numLevels; --i >= 0;)
            //     path.lineTo((float)i, -(levels[(nextSample + i) % numLevels].getStart()));

            // path.closeSubPath();

            // std::cout << nextSample << std::endl;

            // Path p;
            // colourPath(p, segments, numSegments, nextSample);

            //            for (int i = 0; i < numSegments; i++)
            //            {
            //                Path segment;
            //                segment.startNewSubPath((float)i, -(segments[(nextSample + i) % numSegments].getEnd()));
            //                segment.lineTo((float)i, -(segments[(nextSample + i) % numSegments].getStart()));
            //                segment.closeSubPath();
            //
            //                graphic.setColour(colHistory[i]);
            //                graphic.strokePath(segment, PathStrokeType(1.0f), AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(), 0.0f, 1.0f, area.getX(), area.getBottom(), (float)numSegments, -1.0f, area.getRight(), area.getY()));
            //            }

            // graphic.fillPath(p, AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(),
            //                                                 0.0f, 1.0f, area.getX(), area.getBottom(),
            //                                                 (float)numSegments, -1.0f, area.getRight(), area.getY()));
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
