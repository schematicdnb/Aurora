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
        int buffsize = 512;

        colHistory.resize(buffsize);
        //        for (int i = 0; i < 512; i++)
        //        {
        //            colHistory.set(i, Colour::fromRGB(Random::getSystemRandom().nextInt(256),
        //                                              Random::getSystemRandom().nextInt(256),
        //                                              Random::getSystemRandom().nextInt(256)));
        //        }
        setBufferSize(buffsize);
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

    // custom paintchannel backup
    void RGBMeter::paintChannel(Graphics &g, Rectangle<float> area,
                                const Range<float> *levels, int numLevels, int nextSample)
    {

        

        setNextSampleColour(nextSample, bufferColour);

        //                 setNextSampleColour(nextSample, Colour(
        //                                                     Random::getSystemRandom().nextInt(256),
        //                                                     Random::getSystemRandom().nextInt(256),
        //                                                     Random::getSystemRandom().nextInt(256)));

        for (int i = 0; i < numLevels; ++i)
        {

            Path p;
            // get start and end points of segment
            auto p1High = -(levels[(nextSample + i) % numLevels].getEnd());
            auto p1Low = -(levels[(nextSample + i) % numLevels].getStart());

            auto p2High = -(levels[(nextSample + i + 1) % numLevels].getEnd());
            auto p2Low = -(levels[(nextSample + i + 1) % numLevels].getStart());

            // draw segment line
            p.startNewSubPath((float)i, p1High);
            p.lineTo((float)i + 1, p2High);
            p.lineTo((float)i + 1, p2Low);
            p.lineTo((float)i, p1Low);
            p.closeSubPath();

            // colour the segment

            //            g.strokePath(path, PathStrokeType(1.0), AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(), 0.0f, 1.0f, area.getX(), area.getBottom(), (float)numLevels, -1.0f, area.getRight(), area.getY()));

            g.setColour(colHistory[(nextSample + i) % numLevels]);

            //            g.setColour(colHistory[nextSample]);

            g.fillPath(p, AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(),
                                                            0.0f, 1.0f, area.getX(), area.getBottom(),
                                                            (float)numLevels, -1.0f, area.getRight(), area.getY()));
        }
    }
}
