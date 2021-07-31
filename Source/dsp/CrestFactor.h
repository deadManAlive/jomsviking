#pragma once

/*CrestFactor Class:
 * Calculates the average Crest-Factor for a given buffer
 * Crest-Factor is time-variable value calculated from the ratio between peak and rms of the signal
 */

class CrestFactor
{
public:

    CrestFactor() = default;

    // Prepares processor with ProcessSpec-Object and recalculates coefficients for current ballistics
    void prepare(const double& fs);

    // Calculates Crest-Factor for given buffer
    void process(const float* src, int numSamples);

    // Get average calculated attack time of a buffer, call after proces()
    double getAvgAttack();

    // Get average calculated release time of a buffer, call after process()
    double getAvgRelease();

private:
    double attackTimeInSeconds{0.0}, releaseTimeInSeconds{0.14};
    double avgAttackTime{0.0}, avgReleaseTime{0.14};
    double peakState{0.0};
    double rmsState{0.0};
    double a1{0.0}, b1{0.0};
    double sampleRate{0.0};
    double maxAttackTime{0.08}, maxReleaseTime{1.0}; //respective 8ms and 1sec
    double cFactor{0.0};
};
