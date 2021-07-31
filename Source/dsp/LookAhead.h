#pragma once

#include <vector>
#include <juce_dsp/juce_dsp.h>

/*Credits to Daniel Rudrich for the idea https://github.com/DanielRudrich/SimpleCompressor/blob/master/docs/lookAheadLimiter.md */
/*LookAhead-Class:
 *A LookAhead implementation that not only delays the input signal, but also fades in aggressive gain reduction values to avoid distortion*/
class LookAhead
{
public:
    ~LookAhead();
    void prepare(double fs, double delay, int blockSize);
    void process(float* src, int numSamples);

private:
    void pushSamples(const float* src, int numSamples);
    void readSamples(float* dst, int numSamples);
    void processSamples();

    inline void fadeIn(int& index, int range, float& slope, float& nextValue);
    inline void fadeInWithBreak(int& index, int range, float& slope, float& nextValue, bool& procMinimumFound);

    inline void getReadBlockSize(int numSamples, int& index, int& block1, int& block2);
    inline void getWriteBlockSize(int numSamples, int& index, int& block1, int& block2);
    inline void getProcessBlockSize(int numSamples, int index, int& block1, int& block2);
private:
    std::vector<float> buffer;
    float* rawBuffer{nullptr};

    int delayInSamples{0};
    int bufferSize{0};
    int writePosition{0};
    int numLastPushed{0};
};
