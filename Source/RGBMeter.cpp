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
        startTimerHz(60);

        // intialize filter types
        LP.setType(dsp::LinkwitzRileyFilterType::lowpass);
        midHP.setType(dsp::LinkwitzRileyFilterType::highpass);
        midLP.setType(dsp::LinkwitzRileyFilterType::lowpass);
        HP.setType(dsp::LinkwitzRileyFilterType::highpass);

        // Initialize crossover points
        LP.setCutoffFrequency(lowCrossover);
        midHP.setCutoffFrequency(lowCrossover);
        midLP.setCutoffFrequency(highCrossover);
        HP.setCutoffFrequency(highCrossover);

    }

    void RGBMeter::prepare(dsp::ProcessSpec spec)
    {
        sampleRate = spec.sampleRate;
        updateState();

        LP.prepare(spec);
        midLP.prepare(spec);
        midHP.prepare(spec);
        HP.prepare(spec);
    }

    void RGBMeter::updateState() {
        bufferLength = historyLength * sampleRate;
        width = this->getWidth();
        if (width) {
            if (displayBuffer.size() != width)
            {
                displayBuffer.resize(width);
            }
            chunkSize = std::floor(bufferLength / width);
            chunkCounter = 0;
            chunkBuffer = AudioBuffer<float>(1, chunkSize);
        }

    }

    void RGBMeter::pushSamples(AudioBuffer<float> &buffer)
    {

        mainOutputBuffer = &buffer;
        if (!width)
        {
            return;
        } // do nothing, no point in processing buffer if can't be displayed
 

        // Process the incoming buffer
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto sample = buffer.getSample(0, i);
            // add sample to chunk and analysis buffers
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

    void RGBMeter::applyWindowing(AudioBuffer<float> &buffer)
    {
        // create windowing function
        dsp::WindowingFunction<float> windowFunction(buffer.getNumSamples(), dsp::WindowingFunction<float>::blackmanHarris);

        // copy analysis samples to windowed buffer
        windowedBuffer.makeCopyOf(buffer);

        // applying windowing
        windowFunction.multiplyWithWindowingTable(windowedBuffer.getWritePointer(0), windowedBuffer.getNumSamples());
    }

    Colour RGBMeter::colourFreqByFiltering(AudioBuffer<float> &buffer)
    {

        //        applyWindowing(buffer);
        // instantiate band buffers
        AudioBuffer<float> lowBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        AudioBuffer<float> midBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        AudioBuffer<float> highBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        lowBuffer.makeCopyOf(buffer);
        midBuffer.makeCopyOf(buffer);
        highBuffer.makeCopyOf(buffer);

        // DEBUG PROCESSING
        //        lowBuffer.makeCopyOf(windowedBuffer);
        //        midBuffer.makeCopyOf(windowedBuffer);
        //        highBuffer.makeCopyOf(windowedBuffer);

        //         instantiate blocks
        auto lowBlock = dsp::AudioBlock<float>(lowBuffer);
        auto midBlock = dsp::AudioBlock<float>(midBuffer);
        auto highBlock = dsp::AudioBlock<float>(highBuffer);

        // instantiate processing contexts
        auto lowContext = dsp::ProcessContextReplacing<float>(lowBlock);
        auto midContext = dsp::ProcessContextReplacing<float>(midBlock);
        auto highContext = dsp::ProcessContextReplacing<float>(highBlock);

        // process filters
        LP.process(lowContext);
        midHP.process(midContext);
        midLP.process(midContext);
        HP.process(highContext);

        Colour colour = freqToColour(lowBuffer, midBuffer, highBuffer);
        return colour;
    }

    Colour RGBMeter::freqToColour(AudioBuffer<float> &lowBuffer, AudioBuffer<float> &midBuffer, AudioBuffer<float> &highBuffer)
    {

        // get RMS
        auto low = lowBuffer.getRMSLevel(0, 0, lowBuffer.getNumSamples());
        auto mid = midBuffer.getRMSLevel(0, 0, midBuffer.getNumSamples());
        auto high = highBuffer.getRMSLevel(0, 0, highBuffer.getNumSamples());

        mid *= 0.6;

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

        double r = std::floor(low * 255);
        double g = std::floor(mid * 255);
        double b = std::floor(high * 255);

        Colour colour = Colour(r, g, b);

        return colour;
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
            auto max = range.getEnd() * gain;

            auto y1 = jmap(min * gain, -1.0f, 1.0f, (float)this->getHeight(), 0.0f);
            auto y2 = jmap(max * gain, -1.0f, 1.0f, (float)this->getHeight(), 0.0f);

            Path p;
            p.startNewSubPath(i, y1);
            p.lineTo(i, y2);
            p.closeSubPath();

            g.setColour(colour);
            g.strokePath(p, PathStrokeType(1.0f));
//            g.strokePath(p, PathStrokeType(1.0f, PathStrokeType::curved, PathStrokeType::butt));
        }
    }

    Colour RGBMeter::colourFreqByFFT(AudioBuffer<float> &buffer)
    {
        applyWindowing(buffer);

        //        const int fftSize = 2048;
        dsp::FFT fft(log2(freqAnalysisSize));
        std::vector<float> fftData(freqAnalysisSize * 2, 0.0f);

        // Copy buffer data to fftData
        for (int i = 0; i < windowedBuffer.getNumSamples(); ++i)
        {
            fftData[i] = windowedBuffer.getSample(0, i);
        }

        // Perform FFT
        fft.performFrequencyOnlyForwardTransform(fftData.data());

        // Calculate magnitude for each band
        float low = 0.0f, mid = 0.0f, high = 0.0f;
        for (int i = 0; i < freqAnalysisSize / 2; ++i)
        {
            float freq = i * (sampleRate / freqAnalysisSize);
            float magnitude = fftData[i];

            if (freq <= 151.0f)
            {
                low += magnitude;
            }
            else if (freq <= 2000.0f)
            {
                mid += magnitude;
            }
            else if (freq <= 20000.0f)
            {
                high += magnitude;
            }
        }

        // Normalize magnitudes
        float total = low + mid + high;
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

        // Create colour from magnitudes
        Colour colour = Colour(std::floor(low * 255), std::floor(mid * 255), std::floor(high * 255));

        return colour;
    }

    void RGBMeter::resized()
    {
        updateState();
    }

    int RGBMeter::getHistoryLength() {
        return historyLength;
    }
    void RGBMeter::setHistoryLength(int length) {
        historyLength = length;
    }
    float RGBMeter::getGain() {
        return gain;
    }
    void RGBMeter::setGain(float gain) {
        this->gain = Decibels::decibelsToGain(gain / 2);
    }

    void RGBMeter::timerCallback()
    {
        repaint();
    }
}
