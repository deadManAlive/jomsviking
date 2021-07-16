/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

//#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "ff_meters/ff_meters.h"
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

    //meter
    //std::vector<FFAU::LevelMeterSource*> getMeterSource();
    FFAU::LevelMeterSource* getMeterSource(int n);

    //multiband process
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>,
                              juce::dsp::LinkwitzRileyFilter<float>,
                              juce::dsp::LinkwitzRileyFilter<float>,
                              juce::dsp::Gain<float>> fLowBandChain;

    juce::dsp::ProcessorChain<juce::dsp::Gain<float>,
                              juce::dsp::LinkwitzRileyFilter<float>,
                              juce::dsp::LinkwitzRileyFilter<float>,
                              juce::dsp::Gain<float>> fMidBandChain;

    juce::dsp::ProcessorChain<juce::dsp::Gain<float>,
                              juce::dsp::LinkwitzRileyFilter<float>, 
                              juce::dsp::LinkwitzRileyFilter<float>,
                              juce::dsp::Gain<float>> fHIghBandChain;

private:
    //==============================================================================
    //meters
    
    //FFAU::LevelMeterSource meterSource;
    std::array<FFAU::LevelMeterSource, 3> meterSource; //low, mid, high

    enum FilterChain {
        inGain,
        cuttingFilter,
        allPassFilter,
        bandPreGain
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessor)
};

struct ProcessorSettings {
    int oversmpMult{ 1 };

    float mainInGain{ 1.f };

    float tsLXover{ 100.f };
    float tsRXover{ 2000.f };

    float tsLowInGain{ 1.f };
    float tsMidInGain{ 1.f };
    float tsHghInGain{ 1.f };
};

ProcessorSettings getProcessorSettings(juce::AudioProcessorValueTreeState& apvts);

