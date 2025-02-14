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
        //        DBG("initialized");
        startTimer(16);

        slidingWindow.setSize(1, slidingWindowSize);
        //        waveformSamples.insertMultiple(0, std::make_tuple(0.0f, Colour(Colours::black)), displayLength * sampleRate);

        // initialize with full array of default values
        for (int i = 0; i < displayLength * sampleRate; i++)
        {
            waveformSamples.add(std::make_tuple(0.0f, Colour(Colours::black)));
        }
    }

    //    void RGBMeter::prepare(dsp::ProcessSpec spec)
    //    {
    //        waveformSamples.clear();
    //        waveformSamples.insertMultiple(0, std::make_tuple(0.0f, Colour(Colours::black)), displayLength * spec.sampleRate);
    //    }

    void RGBMeter::pushSamples(const AudioBuffer<float> &buffer)
    {
        // skip sliding window and fft for now
        Colour colour = Colour::fromRGB(Random::getSystemRandom().nextInt(256),
                                        Random::getSystemRandom().nextInt(256),
                                        Random::getSystemRandom().nextInt(256));
        DBG(waveformSamples.size());

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto sample = buffer.getSample(1, 0);
            waveformSamples.add(std::make_tuple(sample, colour));
        }
    }

    void RGBMeter::paint(Graphics &g)
    {
        //        DBG("drawing samples");
        auto skip = std::floor(waveformSamples.size() / this->getWidth());

        int index = 0;
        for (int sample = 0; sample < waveformSamples.size() - skip - 1; sample += skip)
        {
            auto level1 = std::get<0>(waveformSamples.get(sample));
            auto colour1 = std::get<1>(waveformSamples.get(sample));

            auto level2 = std::get<0>(waveformSamples.get(sample + skip));
            auto colour2 = std::get<1>(waveformSamples.get(sample + skip));

            Path p;
            p.startNewSubPath(index, jmap(level1, -1.0f, 1.0f, (float)this->getHeight(), (float)0));
            p.lineTo(index + 1, jmap(level2, -1.0f, 1.0f, (float)this->getHeight(), (float)0));
            p.closeSubPath();

            // TODO: colour gradient
            // Create a gradient from colour1 to colour2
            // ColourGradient gradient(colour1, (float)i, jmap(level1, -1.0f, 1.0f, (float)this->getHeight(), (float)0),
            //             colour2, (float)(i+1), jmap(level2, -1.0f, 1.0f, (float)this->getHeight(), (float)0), false);
            // g.setGradientFill(gradient);

            g.setColour(colour1);

            g.strokePath(p, PathStrokeType(1.0f));
            index++;
        }
    }

    void RGBMeter::timerCallback()
    {
        repaint();
    }
}
