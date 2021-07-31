#include "EnvelopeFollower.h"

void EnvelopeFollower::prepare(const double& fs)
{
    sampleRate = fs;
    decayInSamples = static_cast<int>(sampleRate * decayInSecods);
}

void EnvelopeFollower::setDecay(float dcInSeconds)
{
    decayInSecods = dcInSeconds;
    prepare(sampleRate);
}

float EnvelopeFollower::update(float sample)
{
    const double s = static_cast<double>(sample);
    if (s > state) state = s;
    else state *= a;
    return static_cast<float>(state);
}

float EnvelopeFollower::updateLog(float sample)
{
    const double s = static_cast<double>(sample);

    if (s < state) state = s; //attack phase	
    else state *= a; //release phase

    return static_cast<float>(state);
}
