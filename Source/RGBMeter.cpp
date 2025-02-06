//
//  RGBMeter.cpp
//  RGB Meter
//
//  Created by Dan Dubois on 2025-02-06.
//  Copyright © 2025 Schematic Sound. All rights reserved.
//

#include "RGBMeter.h"

namespace juce
{

RGBMeter::RGBMeter() : AudioVisualiserComponent(1)
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

    void RGBMeter::setBufferColour(Colour c)
    {
        bufferColour = c;
    }

    void RGBMeter::setNextSampleColour(int nextSample, Colour c)
    {
        colHistory.set(nextSample, c);
    }

    // paint channel as freq-based colour for whole waveform
    void RGBMeter::paintChannel(Graphics &g, Rectangle<float> area,
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
}
