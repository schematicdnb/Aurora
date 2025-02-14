//
//  RGBMeter.h
//  RGB Meter
//
//  Created by Dan Dubois on 2025-02-06.
//  Copyright © 2025 Schematic Sound. All rights reserved.
//

#include <JuceHeader.h>

namespace juce
{
    class RGBMeter : public AudioVisualiserComponent
    {

    public:
        RGBMeter();

        void setBufferColour(Colour c);
        void setNextSampleColour(int nextSample, Colour c);

        // testing
        // void pushSamples(const float* inputSamples, int numSamples);
        // void paintChannel(Graphics& g, Rectangle<float> area, const float* samples, int numSamples, int nextSample);
        // void paint(Graphics &g);
        

        // void getChannelAsPath(Path &path, const Range<float> *levels, int numLevels, int nextSample);
        void paintChannel(Graphics &g, Rectangle<float> area,
                          const Range<float> *levels, int numLevels, int nextSample) override;

    private:
        // testing
        // AudioBuffer<float> sampleBuffer;
        int bufferSize = 128;
        // int writePosition;


        Array<Colour> colHistory;
        Colour bufferColour;
    };

}
