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

        // initialize with full array of default values
        // for (int i = 0; i < displayLength * sampleRate; i++)
        // {
        //     waveformSamples.add(std::make_tuple(0.0f, Colour(Colours::black)));
        // }

        startTimerHz(60);

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.numChannels = 2;

        // intialize filter types
        LP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        midLP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        midAP.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
        midHP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
        HP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

        // Initialize crossover points
        LP.setCutoffFrequency(150.0f);
        midHP.setCutoffFrequency(150.0f);
        midAP.setCutoffFrequency(2000.0f);
        midLP.setCutoffFrequency(2000.0f);
        HP.setCutoffFrequency(2000.0f);

        // prepare filters
        LP.prepare(spec);
        midLP.prepare(spec);
        midAP.prepare(spec);
        midHP.prepare(spec);
        HP.prepare(spec);

        //        startTimerHz(60);
    }

    void RGBMeter::pushSamples(AudioBuffer<float> &buffer)
    {
        width = this->getWidth();
        if (!width)
        {
            return;
        } // do nothing, no point in processing buffer if can't be displayed

        // initialize/reset the display buffer if it does not match the display width
        if (displayBuffer.size() != width)
        {
            displayBuffer = CircularBuffer<std::tuple<Range<float>, Colour>>(width);
            for (int i = 0; i < width; i++)
            {
                displayBuffer.add(std::make_tuple(Range<float>(0.0f, 0.0f), Colour(Colours::white)));
            }
            chunkSize = std::floor(bufferLength / width); // temp hardcode
            chunkCounter = 0;
            chunkBuffer = AudioBuffer<float>(1, chunkSize);
        }

        // Process the incoming buffer
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto sample = buffer.getSample(0, 0);
            //            waveformSamples.add(std::make_tuple(sample, colour));
            chunkBuffer.setSample(0, chunkCounter, sample);
            freqAnalysisBuffer.add(sample);

            // process display chunk
            if (++chunkCounter == chunkSize)
            {
                // get chunk level range
                auto level = chunkBuffer.findMinMax(0, 0, chunkSize);

                // get colour
                auto analysisAudioBuffer = freqAnalysisBuffer.getBuffer();
                colour = colourFreqByFiltering(analysisAudioBuffer);

                // add chunk to display buffer
                displayBuffer.add(std::make_tuple(level, colour));
                chunkCounter = 0;
            }
        }
    }

    void RGBMeter::paint(Graphics &g)
    {
        if (!width)
        {
            return;
        } // nothing to display

        for (int i = 0; i < displayBuffer.size(); i++)
        {
            auto tuple = displayBuffer.get(i);
            auto range = std::get<0>(tuple);
            auto colour = std::get<1>(tuple);
            auto min = range.getStart();
            auto max = range.getEnd();

            auto y1 = jmap(min, -1.0f, 1.0f, (float)this->getHeight(), 0.0f);
            auto y2 = jmap(max, -1.0f, 1.0f, (float)this->getHeight(), 0.0f);

            Path p;
            p.startNewSubPath(i, y1);
            p.lineTo(i, y2);
            p.closeSubPath();
            
            

            g.setColour(colour);
            g.strokePath(p, PathStrokeType(0.5f, PathStrokeType::curved, PathStrokeType::rounded));
        }
    }

    Colour RGBMeter::colourFreqByFiltering(AudioBuffer<float> &buffer)
    {
        // instantiate band buffers
        AudioBuffer<float> lowBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        AudioBuffer<float> midBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        AudioBuffer<float> highBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        lowBuffer = buffer;
        midBuffer = buffer;

        // instantiate blocks
        auto lowBlock = dsp::AudioBlock<float>(lowBuffer);
        auto midBlock = dsp::AudioBlock<float>(midBuffer);
        auto highBlock = dsp::AudioBlock<float>(highBuffer);

        // instantiate processing contexts
        auto lowContext = dsp::ProcessContextReplacing<float>(lowBlock);
        auto midContext = dsp::ProcessContextReplacing<float>(midBlock);
        auto highContext = dsp::ProcessContextReplacing<float>(highBlock);

        // process filters
        LP.process(lowContext);
        midAP.process(lowContext);

        midHP.process(midContext);
        highBuffer = midBuffer;
        midLP.process(midContext);

        HP.process(highContext);

        Colour colour = freqToColour(lowBuffer, midBuffer, highBuffer);
        return colour;
    }

    Colour RGBMeter::freqToColour(AudioBuffer<float> &lowBuffer, AudioBuffer<float> &midBuffer, AudioBuffer<float> &highBuffer)
    {

        // get highest magnitude for each frequency band
        //                auto low = lowBuffer.getMagnitude(0, lowBuffer.getNumSamples());
        //                auto mid = midBuffer.getMagnitude(0, midBuffer.getNumSamples());
        //                auto high = highBuffer.getMagnitude(0, highBuffer.getNumSamples());

        // get RMS
        auto low = lowBuffer.getRMSLevel(0, 0, lowBuffer.getNumSamples());
        auto mid = midBuffer.getRMSLevel(0, 0, midBuffer.getNumSamples());
        auto high = highBuffer.getRMSLevel(0, 0, highBuffer.getNumSamples());

        auto total = low + mid + high;

        // normalize
        if (total > 0.0f)
        {
            low /= total;
            mid /= total;
            high /= total;
        }

        // Scale low, mid, high by an equal factor such that none go higher than 1
            float maxComponent = std::max({low, mid, high});
            low /= maxComponent;
            mid /= maxComponent;
            high /= maxComponent;

        Colour colour = Colour(std::floor(low * 255), std::floor(mid * 255), std::floor(high * 255));

        return colour;
    }

    Colour RGBMeter::colourFreqByFFT(AudioBuffer<float> &buffer)
    {
        //                colour = colourFreqByFFT(analysisAudioBuffer);

        // Process frequency to colour
        // buggy fft implementation
        //                dsp::FFT fft(11); // 2^11 = 2048
        //                std::vector<float> fftData(fftSize * 2);
        //                for (int j = 0; j < fftSize; j++)
        //                {
        //                    fftData[j] = fftBuffer.get(j);
        //                }
        //
        //                fft.performFrequencyOnlyForwardTransform(fftData.data());
        //
        //                auto low = 0.0f;
        //                auto mid = 0.0f;
        //                auto high = 0.0f;
        //
        //                const float binWidth = (float)(sampleRate / fftSize);
        //                for (int j = 0; j < fftSize; ++j)
        //                {
        //
        //
        //                    auto freq = j * binWidth;
        //                    auto mag = fftData[j];
        //                    if (freq < 151)
        //                    {
        //                        low += mag;
        //                    }
        //                    else if (freq < 2000)
        //                    {
        //                        mid += mag;
        //                    }
        //                    else if (freq < 20000)
        //                    {
        //                        high += mag;
        //                    }
        //                }
        //
        //                auto total = low + mid + high;
        //
        //                low /= total;
        //                mid /= total;
        //                high /= total;
        //
        //                colour = Colour(std::floor(low * 255), std::floor(mid * 255), std::floor(high * 255));
    }

    void RGBMeter::timerCallback()
    {
        repaint();
    }
}
