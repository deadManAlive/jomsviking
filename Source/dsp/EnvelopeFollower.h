#pragma once

/*Basic sample-wise envenlope follower with instant attack*/
class EnvelopeFollower
{
public:

    EnvelopeFollower() = default;

    // Prepares the the envelope follower with given sample rate
    void prepare(const double& fs);

    // Sets decay/release time constant
    void setDecay(float dcInSeconds);

    // Updates envelope follower, use buffer that contains linear values i.e. 0.0-1.0
    float update(float sample);

    // Updates envelope follower, use when buffer containts logarithmic values i.e. -infdB - 0dB
    float updateLog(float sample);

private:
    double decayInSecods{0.0};
    int decayInSamples{0};
    double a{1.0};
    double sampleRate{0.0};
    double state{0.0};
};
