#include "LevelEnvelopeFollower.h"

#include <cassert>
#include <cmath>

void LevelEnvelopeFollower::prepare(const double& fs)
{
    sampleRate = fs;

    peakDecayInSamples = static_cast<int>(peakDecayInSeconds * sampleRate);
    peakDecay = 1.0f - 1.0f / static_cast<float>(peakDecayInSamples);

    rmsDecayInSamples = static_cast<int>(rmsDecayInSeconds * sampleRate);
    rmsDecay = 1.0f - 1.0f / static_cast<float>(rmsDecayInSamples);
}

// Set peak decay
void LevelEnvelopeFollower::setPeakDecay(float dc)
{
    peakDecayInSeconds = dc;
    prepare(sampleRate);
}

// Set rms decay
void LevelEnvelopeFollower::setRmsDecay(float dc)
{
    rmsDecayInSeconds = dc;
    prepare(sampleRate);
}

// Updates peak envelope follower from given audio buffer
void LevelEnvelopeFollower::updatePeak(const float* const* channelData, int numChannels, int numSamples)
{
    assert(numChannels >= 0 && numSamples >= 0 && channelData != nullptr);
    if (numChannels > 0 && numSamples > 0)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float sum = 0.0f;
            for (int j = 0; j < numChannels; ++j)
                sum += std::abs(channelData[j][i]);

            sum /= static_cast<float>(numChannels);

            if (sum > currMaxPeak)
                currMaxPeak = sum;
            else if (currMaxPeak > 0.001f)
                currMaxPeak *= peakDecay;
            else currMaxPeak = 0.0f;
        }
    }
}

// Updates rms envelope follower from given audio buffer
void LevelEnvelopeFollower::updateRMS(const float* const* channelData, int numChannels, int numSamples)
{
    assert(numChannels >= 0 && numSamples >= 0 && channelData != nullptr);
    if (numChannels > 0 && numSamples > 0)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float sum = 0.0f;
            for (int j = 0; j < numChannels; ++j)
                sum += std::abs(channelData[j][i]);

            sum /= static_cast<float>(numChannels);
            sum *= sum;

            if (sum > currMaxRMS)
                currMaxRMS = sum * rmsDecay;
            else if (currMaxRMS > 0.001f)
                currMaxRMS *= peakDecay;
            else currMaxRMS = 0.0f;
        }
    }
}

// Gets current peak, call after updatePeak
float LevelEnvelopeFollower::getPeak()
{
    return currMaxPeak;
}

// Gets current rms, vall after updateRMS
float LevelEnvelopeFollower::getRMS()
{
    return sqrt(currMaxRMS);
}
