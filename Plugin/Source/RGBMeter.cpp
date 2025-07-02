//
//  RGBMeter.cpp
//  RGB Meter
//
//  Created by Dan Dubois on 2025-02-06.
//  Copyright © 2025 Schematic Sound. All rights reserved.
//

#include "RGBMeter.h"

RGBMeter::RGBMeter() {

    vBlankAttachment = std::make_unique<VBlankAttachment>(this, [this] {repaint();});

    // intialize filter types
    LP.setType(dsp::LinkwitzRileyFilterType::lowpass);
    midHP.setType(dsp::LinkwitzRileyFilterType::highpass);
    midLP.setType(dsp::LinkwitzRileyFilterType::lowpass);
    HP.setType(dsp::LinkwitzRileyFilterType::highpass);

    corners.setCornerRadius(cornerRadius);
    this->setComponentEffect(&corners);
}

void RGBMeter::prepare(dsp::ProcessSpec spec)
{
    sampleRate = spec.sampleRate;
    updateState();

    LP.prepare(spec);
    LP.reset();
    
    midLP.prepare(spec);
    midLP.reset();
    
    midHP.prepare(spec);
    midHP.reset();
    
    HP.prepare(spec);
    HP.reset();
}

void RGBMeter::updateState()
{
    bufferLength = historyLength * sampleRate;
    width = this->getWidth();
    if (!width) return;
    if (displayBuffer.size() != width)
    {
        displayBuffer.resize(width);
    }
    chunkSize = std::floor(bufferLength / width);
    chunkCounter = 0;
    chunkBuffer = AudioBuffer<float>(1, std::max(0.0, chunkSize));
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
        auto sample = buffer.getSample(displayChannel, i);
        // add sample to chunk and analysis buffers
        if (chunkBuffer.getNumChannels() > 0 && chunkBuffer.getNumSamples() > 0) {
            chunkBuffer.setSample(0, chunkCounter, sample);
        }
        
        freqAnalysisBuffer.add(sample);

        // process display chunk
        if (++chunkCounter == chunkSize)
        {
            // get chunk level range
            auto level = chunkBuffer.findMinMax(0, 0, chunkSize);

            // get colour
            auto analysisAudioBuffer = freqAnalysisBuffer.getBuffer();
            auto colour = colourFreqByFiltering(analysisAudioBuffer);

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
    
    // Cut low by 3dB, boost high by 1.5dB
    low *= Decibels::decibelsToGain(-3);
    high *= Decibels::decibelsToGain(1.5);

    // apply colour weighting
    low *= lowWeight;
    mid *= midWeight;
    high *= highWeight;

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
        auto max = range.getEnd();

        if (std::isnan(min) || std::isnan(max))
        {
            DBG("NaN");
            continue;
        }

        auto y1 = jmap(min * gain, -1.0f, 1.0f, (float)this->getHeight(), 0.0f);
        auto y2 = jmap(max * gain, -1.0f, 1.0f, (float)this->getHeight(), 0.0f);

        if (std::isnan(y1) || std::isnan(y2))
        {
            DBG("NaN");
            continue;
        }

        g.setColour(colour);
        g.drawVerticalLine(i, std::min(y1,y2), std::max(y1,y2));
    }

}

void RGBMeter::resized()
{
    updateState();
}

int RGBMeter::getHistoryLength()
{
    return historyLength;
}
void RGBMeter::setHistoryLength(int length)
{
    historyLength = length;
}
float RGBMeter::getGain()
{
    return gain;
}
void RGBMeter::setGain(float gain)
{
    this->gain = Decibels::decibelsToGain(gain);
}
void RGBMeter::setLowCrossover(float freq)
{
    LP.setCutoffFrequency(freq);
    midHP.setCutoffFrequency(freq);
}
void RGBMeter::setHighCrossover(float freq)
{
    midLP.setCutoffFrequency(freq);
    HP.setCutoffFrequency(freq);
}
float RGBMeter::getCornerRadius()
{
    return cornerRadius;
}
void RGBMeter::setColourWeight(String colour, float weight)
{
    if (colour == "red")
    {
        lowWeight = weight;
    }
    else if (colour == "green")
    {
        midWeight = weight;
    }
    else if (colour == "blue")
    {
        highWeight = weight;
    }
}
void RGBMeter::setDisplayChannel(bool isRightChannel) {
    displayChannel = isRightChannel && mainOutputBuffer->getNumChannels() == 2 ? 1 : 0;
}
