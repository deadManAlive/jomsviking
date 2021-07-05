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

    //state
    static juce::AudioProcessorValueTreeState::ParameterLayout paramLayoutGen();

    juce::AudioProcessorValueTreeState processTreeState = {*this, nullptr, "prm_state", paramLayoutGen()};

    //assets
    void updateProcessorChains();

    //std::atomic<float> pCrossoverLM;    //Low-Mid crossover
    //std::atomic<float> pcrossoverMH;    //Mid-Hi crossover

    //std::atomic<float> pBandGainLow;    //Low Band Gain
    //std::atomic<float> pBandGainMid;
    //std::atomic<float> pBandGainHigh;   //High Band Gain

    //crossover + input gain process
    juce::dsp::ProcessorChain<juce::dsp::LinkwitzRileyFilter<float>, juce::dsp::LinkwitzRileyFilter<float>, juce::dsp::Gain<float>> fLowBandChain;
    juce::dsp::ProcessorChain<juce::dsp::LinkwitzRileyFilter<float>, juce::dsp::LinkwitzRileyFilter<float>, juce::dsp::Gain<float>> fMidBandChain;
    juce::dsp::ProcessorChain<juce::dsp::LinkwitzRileyFilter<float>, juce::dsp::LinkwitzRileyFilter<float>, juce::dsp::Gain<float>> fHIghBandChain;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessor)
};

struct ProcessorSettings {
    float tsLXover{100.f};
    float tsRXover{2000.f};

    float tsLowInGain{1.f};
    float tsMidInGain{1.f};
    float tsHghInGain{1.f};

    int oversmpMult{ 1 };
};

ProcessorSettings getProcessorSettings(juce::AudioProcessorValueTreeState& apvts);

