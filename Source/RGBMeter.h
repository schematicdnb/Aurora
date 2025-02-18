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

    class RGBMeter : public Component, Timer
    {

    public:
        RGBMeter();

        void pushSamples(const AudioBuffer<float> &buffer);
        void paint(Graphics &g) override;
        void timerCallback() override;
        Colour freqToColour(AudioBuffer<float> &lowBuffer, AudioBuffer<float> &midBuffer, AudioBuffer<float> &highBuffer);

    private:
        //        AudioDeviceManager device;
        //        int slidingWindowSize = 2048;
        int displayLength = 5;  // in seconds
        int sampleRate = 44100; // temp hardcode
        int bufferLength = displayLength * sampleRate;

        int width = 0;
        // int offset = 0;

        AudioBuffer<float> chunkBuffer;
        double chunkSize;
        double chunkCounter;

        int freqAnalysisSize = 4096;
        AudioBuffer<float> freqBuffer{1, freqAnalysisSize};
        int freqBufferCounter = 0;
        //        int fftCounter =
        //        AudioBuffer<float> slidingWindow;
        // CircularBuffer<std::tuple<float, Colour>> waveformSamples{bufferLength};

        CircularBuffer<std::tuple<Range<float>, Colour>> displayBuffer{0};
        int fftSize = 2048;
        CircularBuffer<float> fftBuffer{fftSize};

        using Filter = juce::dsp::LinkwitzRileyFilter<float>;
        Filter LP, midLP, midAP, midHP, HP;
        
        Colour colour;
    };
    ;

}
