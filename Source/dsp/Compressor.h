#pragma once

#include "LevelDetector.h"
#include "GainComputer.h"
#include "DelayLine.h"
#include "LookAhead.h"

#include <juce_dsp/juce_dsp.h>

/* Compressor-Class:
 * The circruit is modeled after the "ideal" VCA-Compressor
 * based on the paper "Digital Dynamic Range Compressor Design �  Tutorial and Analysis"
 * by Giannoulis, Massberg & Reiss
 */

class Compressor
{
public:

    Compressor() = default;
    ~Compressor();

    // Prepares compressor with a ProcessSpec-Object containing samplerate, blocksize and number of channels
    void prepare(const juce::dsp::ProcessSpec& ps);

    // Sets compressor to bypassed/not bypassed
    void setPower(bool);

    // Sets input in dB
    void setInput(float);

    // Sets threshold in dB
    void setThreshold(float);

    // Sets ratio in dB
    void setRatio(float);

    // Sets knee-width in dB (> 0 = soft knee)
    void setKnee(float);

    // Sets make-up gain in dB
    void setMakeup(float);

    // Sets mix 0.0f - 1.0f
    void setMix(float);

    // Sets attack time in milliseconds
    void setAttack(float);

    // Sets release time in milliseconds
    void setRelease(float);

    // Sets auto attack to enabled = true or disabled = false
    void setAutoAttack(bool);

    // Sets auto release to enabled = true or disabled = false
    void setAutoRelease(bool);

    // Sets auto makeup to enabled = true or disabled = false
    void setAutoMakeup(bool);

    // Enables lookahead
    void setLookahead(bool);

    // Gets current make-up gain value
    float getMakeup();

    // Return current sampleRate
    double getSampleRate();

    float getMaxGainReduction();

    // Processes input buffer
    void process(juce::AudioBuffer<float>& buffer);

private:
    inline void applyInputGain(juce::AudioBuffer<float>&, int);
    inline float calculateAutoMakeup(const float*, int numSamples);

    //Directly initialize process spec to avoid debugging problems
    juce::dsp::ProcessSpec procSpec{-1, 0, 0};

    juce::AudioBuffer<float> originalSignal;
    std::vector<float> sidechainSignal;
    float* rawSidechainSignal{nullptr};

    LevelDetector ballistics;
    GainComputer gainComputer;
    DelayLine delay;
    LookAhead lookahead;
    SmoothingFilter smoothedAutoMakeup;

    double lookaheadDelay{0.005};
    float input{0.0f};
    float prevInput{0.0f};
    float makeup{0.0f};
    float autoMakeup{0.0f};
    bool bypassed{false};
    bool lookaheadEnabled{false};
    bool autoMakeupEnabled{false};
    float mix{1.0f};
    float maxGainReduction{0.0f};
};
