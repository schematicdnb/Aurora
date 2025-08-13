//
//  RGBMeter.h
//  RGB Meter
//
//  Created by Dan Dubois on 2025-02-06.
//  Copyright © 2025 Schematic Sound. All rights reserved.
//

#include <JuceHeader.h>
#include "RoundedCornersEffect.h"

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer(int size) : buffer(size), head(0), tail(0), full(false)
    {
    }

    void add(T item)
    {
        buffer[head] = item;
        if (full)
        {
            tail = (tail + 1) % buffer.size();
        }
        head = (head + 1) % buffer.size();
        full = head == tail;
    }

    AudioBuffer<T> getBuffer() const
    {
        AudioBuffer<T> audioBuffer(1, std::max(0, (int)size()));
        if (empty())
        {
            return audioBuffer;
        }

        size_t current = tail;
        for (int i = 0; i < size(); i++)
        {
            audioBuffer.setSample(0, i, buffer[current]);
            current = (current + 1) % buffer.size();
        }

        return audioBuffer;
    }

    void resize(size_t size)
    {
        buffer.resize(size);
        head = 0;
        tail = head;
        full = true;
    }

    T get(size_t index) const
    {
        if (index >= size())
        {
            throw std::out_of_range("Index out of range");
        }
        return buffer[(tail + index) % buffer.size()];
    }

    void replace(size_t index, T item)
    {
        if (index >= size())
        {
            throw std::out_of_range("Index out of range");
        }
        buffer[(tail + index) % buffer.size()] = item;
    }
    

    size_t size() const
    {
        if (full)
        {
            return buffer.size();
        }
        if (head >= tail)
        {
            return head - tail;
        }
        return buffer.size() + head - tail;
    }

    bool empty() const
    {
        return (!full && (head == tail));
    }

    bool isFull() const
    {
        return full;
    }

private:
    std::vector<T> buffer;
    size_t head;
    size_t tail;
    bool full;
};

class RGBMeter : public Component
{

public:
    RGBMeter();

    void prepare(dsp::ProcessSpec spec);
    void updateState();
    void pushSamples(AudioBuffer<float> &buffer);
    void paint(Graphics &g) override;
    Colour freqToColour(AudioBuffer<float> &lowBuffer, AudioBuffer<float> &midBuffer, AudioBuffer<float> &highBuffer);
    Colour colourFreqByFiltering(AudioBuffer<float> &buffer);

    void applyWindowing(AudioBuffer<float> &buffer);

    void resized() override;
    int getHistoryLength();
    void setHistoryLength(int length);
    float getGain();
    void setGain(float gain);
    
    void setLowCrossover(float freq);
    void setHighCrossover(float freq);
    
    float getCornerRadius();
    
    void setColourWeight(String colour, float weight);
    void setDisplayChannel(bool isRightChannel);
    
    void toggleFreezeWaveform();
    
private:
    std::unique_ptr<VBlankAttachment> vBlankAttachment;

    int historyLength = 4; // in seconds
    int sampleRate;
    int bufferLength;
    Atomic<int> displayChannel{0};

    int width = 0;

    AudioBuffer<float> chunkBuffer{1, 0};
    double chunkSize;
    double chunkCounter;

    int freqAnalysisSize = 2048;
    CircularBuffer<float> freqAnalysisBuffer{freqAnalysisSize};
    AudioBuffer<float> windowedBuffer{1, 0};

    CircularBuffer<std::tuple<Range<float>, Colour>> displayBuffer{0};

    using Filter = dsp::LinkwitzRileyFilter<float>;
    Filter LP, midLP, midAP, midHP, HP;
    
    float gain;

    AudioBuffer<float> *mainOutputBuffer;
    
    RoundedCornersEffect corners;
    float cornerRadius = 7;
    
    float lowWeight;
    float midWeight;
    float highWeight;
    
    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RGBMeter)
};

