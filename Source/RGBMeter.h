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
        
    private:
        Array<Colour> colHistory;
        Colour bufferColour;
        
    public:
        RGBMeter();

        void setBufferColour(Colour c);
        void setNextSampleColour(int nextSample, Colour c);

        // paint channel as freq-based colour for whole waveform
        void paintChannel(Graphics &g, Rectangle<float> area,
                          const Range<float> *levels, int numLevels, int nextSample);
    };
}
