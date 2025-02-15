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

    RGBMeter::RGBMeter()
    {

        slidingWindow.setSize(1, slidingWindowSize);

        // initialize with full array of default values
        for (int i = 0; i < displayLength * sampleRate; i++)
        {
            waveformSamples.add(std::make_tuple(0.0f, Colour(Colours::black)));
        }

        // auto writePointer = waveformSamples.getWritePointer();

        startTimerHz(60);
    }

    //    void RGBMeter::prepare(dsp::ProcessSpec spec)
    //    {
    //        waveformSamples.clear();
    //        waveformSamples.insertMultiple(0, std::make_tuple(0.0f, Colour(Colours::black)), displayLength * spec.sampleRate);
    //    }

    void RGBMeter::pushSamples(const AudioBuffer<float> &buffer)
    {
        // skip sliding window and fft for now

        //        Colour colour = Colour(Colours::white);
        Colour colour = Colour::fromRGB(Random::getSystemRandom().nextInt(256),
                                        Random::getSystemRandom().nextInt(256),
                                        Random::getSystemRandom().nextInt(256));

        skip = std::floor(waveformSamples.size() / this->getWidth()); // temporary

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto sample = buffer.getSample(1, 0);
            waveformSamples.add(std::make_tuple(sample, colour));
            if (offset == 0)
            {
                offset = skip - 1;
            }
            else
            {
                offset--;
            }
        }
    }

    void RGBMeter::paint(Graphics &g)
    {
        //        DBG("drawing samples");
        //        auto skip = std::floor(waveformSamples.size() / this->getWidth());

        int x = 0;
        for (int sample = offset; sample < waveformSamples.size() - skip; sample += skip)
        {

            auto min = std::numeric_limits<float>::max();
            auto max = std::numeric_limits<float>::lowest();
            for (int i = sample; i <= sample + skip; ++i)
            {
                auto level = std::get<0>(waveformSamples.get(i));
                if (level < min)
                {
                    min = level;
                }
                if (level > max)
                {
                    max = level;
                }
            }

            auto colour1 = std::get<1>(waveformSamples.get(sample));

            // auto level2 = std::get<0>(waveformSamples.get(sample + skip));
            auto colour2 = std::get<1>(waveformSamples.get(sample + skip));

            auto y1 = jmap(min, -1.0f, 1.0f, (float)this->getHeight(), (float)0);
            auto y2 = jmap(max, -1.0f, 1.0f, (float)this->getHeight(), (float)0);

            Path p;
            p.startNewSubPath(x, y1);
            p.lineTo(x, y2);
            p.closeSubPath();

            // TODO: colour gradient
//            //             Create a gradient from colour1 to colour2
//            ColourGradient gradient(colour1, x, y1,
//                                    colour2, x+1, y2, false);
////            g.setGradientFill(gradient);
////            g.fillPath(p);
//            GradientPathStrokeType gradientStroke(1.0f, gradient);
//            g.strokePath(p, gradientStroke);

            g.setColour(colour1);
            g.strokePath(p, PathStrokeType(1.0f));

            
            x++;
        }
    }

    void RGBMeter::timerCallback()
    {
        repaint();
    }
}
