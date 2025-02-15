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
        CircularBuffer(int size) : buffer(size), head(0), tail(0), full(false) {
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

        // T* getWritePointer()
        // {
        //     return &buffer[head];
        // }

        // void incrementWritePointer()
        // {
        //     head = (head + 1) % buffer.size();
        //     if (full)
        //     {
        //         tail = (tail + 1) % buffer.size();
        //     }
        //     full = head == tail;
        // }

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

    class GradientPathStrokeType : public PathStrokeType
    {
    public:
        GradientPathStrokeType(float thickness, ColourGradient gradient)
            : PathStrokeType(thickness), gradient(gradient)
        {
        }

        void createStrokedPath(Path& destPath, const Path& sourcePath, const AffineTransform& transform = AffineTransform()) const
        {
            PathStrokeType::createStrokedPath(destPath, sourcePath, transform);

            Path::Iterator it(destPath);
            while (it.next())
            {
                if (it.elementType == Path::Iterator::lineTo || it.elementType == Path::Iterator::cubicTo || it.elementType == Path::Iterator::quadraticTo)
                {
                    float position = Point<float>(it.x1, it.y1).getDistanceFrom(sourcePath.getPointAlongPath(0.0f)) / sourcePath.getLength();
                    Colour colour = gradient.getColourAtPosition(position);
                    destPath.addLineSegment(Line<float>(it.x1, it.y1, it.x2, it.y2), getStrokeThickness());
                    destPath.applyTransform(AffineTransform::translation(it.x1, it.y1));
                }
            }
        }

    private:
        ColourGradient gradient;
    };

    class RGBMeter : public Component, Timer
    {

    public:
        RGBMeter();

//        void prepare(dsp::ProcessSpec spec);
        void pushSamples(const AudioBuffer<float> &buffer);
        //        void pushTuple(float level, Colour colour);
        void paint(Graphics &g) override;
        

    private:
//        AudioDeviceManager device;
        int slidingWindowSize = 2048;
        int displayLength = 3;  // in seconds
        int sampleRate = 44100; // temporary
        double skip;
        int offset = 0;
        AudioBuffer<float> slidingWindow;
        // Array<std::tuple<float, Colour>> waveformSamples;
        CircularBuffer<std::tuple<float, Colour>> waveformSamples{displayLength * sampleRate};
        void timerCallback() override;
    };
    ;

}
