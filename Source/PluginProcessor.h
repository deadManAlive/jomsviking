/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class JomsvikingAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    JomsvikingAudioProcessor();
    ~JomsvikingAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //=====assets
    void updateProcessorChains();

    std::atomic<float> pCrossoverLM;    //Low-Mid crossover
    std::atomic<float> pcrossoverMH;    //Mid-Hi crossover

    std::atomic<float> pBandGainLow;    //Low Band Gain
    std::atomic<float> pBandGainMid;
    std::atomic<float> pBandGainHigh;   //High Band Gain

    //1st stage
    juce::dsp::LinkwitzRileyFilter<float> fFirstStageLowPass;
    juce::dsp::LinkwitzRileyFilter<float> fFirstStageHighPass;
    //2nd stage
    juce::dsp::LinkwitzRileyFilter<float> fScndStageLowPass;
    juce::dsp::LinkwitzRileyFilter<float> fScndStageHighPass;
    //3rd stage
    juce::dsp::LinkwitzRileyFilter<float> fLowBandAPF;

    //gain
    juce::dsp::Gain<float> mLowBandGain;
    juce::dsp::Gain<float> mMidBandGain;
    juce::dsp::Gain<float> mHighBandGain;

private:
    double mSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessor)
};
