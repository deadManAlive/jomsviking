#include "CrestFactor.h"

#include <juce_core/juce_core.h>

void CrestFactor::prepare(const double& fs)
{
    sampleRate = fs;
    //Calculate alpha for release time of 200ms, same release time for peak & rms detector
    a1 = exp(-1.0 / (sampleRate * 0.2));
    b1 = 1 - a1;
}


void CrestFactor::
process(const float* src, const int numSamples)
{
    //Init accumulators
    if (!peakState) peakState = src[0];
    if (!rmsState) rmsState = src[0];

    //Reset avg attack/release
    avgAttackTime = 0.0;
    avgReleaseTime = 0.0;

    //Calculate averages of auto - attack/release times for a single buffer
    for (int i = 0; i < numSamples; ++i)
    {
        //Square of input signal
        const double s = static_cast<double>(src[i]) * static_cast<double>(src[i]);

        //Update peak state
        peakState = juce::jmax(s, a1 * peakState + b1 * s);

        //Update rms state
        rmsState = a1 * rmsState + b1 * s;

        //calculate squared crest factor
        const double c = peakState / rmsState;
        cFactor = c > 0.0 ? c : 0.0;

        //calculate ballistics
        if (cFactor > 0.0)
        {
            attackTimeInSeconds = 2 * (maxAttackTime / cFactor);
            releaseTimeInSeconds = 2 * (maxReleaseTime / cFactor) - attackTimeInSeconds;

            //Update avg ballistics
            avgAttackTime += attackTimeInSeconds;
            avgReleaseTime += releaseTimeInSeconds;
        }
    }

    // Calculate average ballistics & crest factor
    avgAttackTime /= numSamples;
    avgReleaseTime /= numSamples;
}

double CrestFactor::getAvgAttack()
{
    return avgAttackTime;
}

double CrestFactor::getAvgRelease()
{
    return avgReleaseTime;
}
