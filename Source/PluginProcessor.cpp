/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JomsvikingAudioProcessor::JomsvikingAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    pBandGainLow = 1.0;
    pBandGainHigh = 1.0;
    pBandGainMid = 1.0;

    pcrossoverMH = 2000.0;
    pCrossoverLM = 100.0;

    fLowBandChain.get<0>().setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    fLowBandChain.get<1>().setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    fMidBandChain.get<0>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    fMidBandChain.get<1>().setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

    fHIghBandChain.get<0>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    fHIghBandChain.get<1>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

JomsvikingAudioProcessor::~JomsvikingAudioProcessor()
{
}

//==============================================================================
const juce::String JomsvikingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JomsvikingAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JomsvikingAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JomsvikingAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JomsvikingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JomsvikingAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JomsvikingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JomsvikingAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JomsvikingAudioProcessor::getProgramName (int index)
{
    return {};
}

void JomsvikingAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JomsvikingAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec proSpec;
    proSpec.sampleRate = sampleRate;
    proSpec.maximumBlockSize = samplesPerBlock;
    proSpec.numChannels = getMainBusNumOutputChannels();

    updateProcessorChains();

    fLowBandChain.prepare(proSpec);
    fMidBandChain.prepare(proSpec);
    fHIghBandChain.prepare(proSpec);
}

void JomsvikingAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JomsvikingAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JomsvikingAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto bufferLength = buffer.getNumSamples();
    updateProcessorChains();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    juce::AudioBuffer<float> bandBufferLow;
    juce::AudioBuffer<float> bandBufferHigh;
    juce::AudioBuffer<float> bandBufferMid;

    bandBufferLow.setSize(totalNumInputChannels, bufferLength);
    bandBufferMid.setSize(totalNumInputChannels, bufferLength);
    bandBufferHigh.setSize(totalNumInputChannels, bufferLength);

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        auto* bufferData = buffer.getReadPointer(channel);

        bandBufferLow.copyFrom(channel, 0, bufferData, bufferLength);
        bandBufferMid.copyFrom(channel, 0, bufferData, bufferLength);
        bandBufferHigh.copyFrom(channel, 0, bufferData, bufferLength);
    }

    juce::dsp::AudioBlock<float> bandBlockLow(bandBufferLow);
    juce::dsp::AudioBlock<float> bandBlockMid(bandBufferMid);
    juce::dsp::AudioBlock<float> bandBlockHigh(bandBufferHigh);

    fLowBandChain.process(juce::dsp::ProcessContextReplacing<float>(bandBlockLow));
    fMidBandChain.process(juce::dsp::ProcessContextReplacing<float>(bandBlockMid));
    fHIghBandChain.process(juce::dsp::ProcessContextReplacing<float>(bandBlockHigh));

    buffer.clear();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* bandDataLow = bandBufferLow.getReadPointer(channel);
        auto* bandDataMid = bandBufferMid.getReadPointer(channel);
        auto* bandDataHigh = bandBufferHigh.getReadPointer(channel);

        buffer.addFrom(channel, 0, bandDataLow, bufferLength);
        buffer.addFrom(channel, 0, bandDataMid, bufferLength);
        buffer.addFrom(channel, 0, bandDataHigh, bufferLength);
    }
}

//==============================================================================
bool JomsvikingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JomsvikingAudioProcessor::createEditor()
{
    //return new JomsvikingAudioProcessorEditor (*this); //switch to get jomsviking GUI
    return new juce::GenericAudioProcessorEditor(*this);

}

//==============================================================================
void JomsvikingAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JomsvikingAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout JomsvikingAudioProcessor::paramLayoutGen() {
    juce::AudioProcessorValueTreeState::ParameterLayout playout;

    std::unique_ptr<juce::AudioParameterFloat> tsCrossoverLM(new juce::AudioParameterFloat("lcrossover", "Lower Crossover", 30.f, 2000.f, 1000.f));
    std::unique_ptr<juce::AudioParameterFloat> tsCrossoverMH(new juce::AudioParameterFloat("rcrossover", "Upper Crossover", 2000.f, 16000.f, 4000.f));

    std::unique_ptr<juce::AudioParameterFloat> tsInGainMultLow(new juce::AudioParameterFloat("iGainLow", "Low Band Input Gain", juce::NormalisableRange<float>(0.f, 2.f, 0.01f, 0.25f), 1.f));
    std::unique_ptr<juce::AudioParameterFloat> tsInGainMultMid(new juce::AudioParameterFloat("iGainMid", "Mid Band Input Gain", juce::NormalisableRange<float>(0.f, 2.f, 0.01f, 0.25f), 1.f));
    std::unique_ptr<juce::AudioParameterFloat> tsInGainMultHgh(new juce::AudioParameterFloat("iGainHgh", "High Band Input Gain", juce::NormalisableRange<float>(0.f, 2.f, 0.01f, 0.25f), 1.f));

    juce::StringArray oversamplingOptStr;
    oversamplingOptStr.add("No Oversampling");
    for (int i = 1; i <= 6; i++) {
        juce::String opt;
        opt << pow(2, i);
        opt << "x";
        oversamplingOptStr.add(opt);
    }

    std::unique_ptr<juce::AudioParameterChoice> oversamplingOpt(new juce::AudioParameterChoice("oversampling", "Oversamplping", oversamplingOptStr, 0));

    playout.add(std::move(tsCrossoverLM),
                std::move(tsCrossoverMH),
                std::move(tsInGainMultLow),
                std::move(tsInGainMultMid),
                std::move(tsInGainMultHgh),
                std::move(oversamplingOpt)
                );

    return playout;
}

void JomsvikingAudioProcessor::updateProcessorChains() {
    float crossoverLM = pCrossoverLM;
    float crossoverMH = pcrossoverMH;

    float fBandGainLow = pBandGainLow;
    float fBandGainMid = pBandGainMid;
    float fBandGainHigh = pBandGainHigh;

    fLowBandChain.get<0>().setCutoffFrequency(crossoverLM);
    fLowBandChain.get<1>().setCutoffFrequency(crossoverMH);
    fLowBandChain.get<2>().setGainLinear(fBandGainLow);

    fMidBandChain.get<0>().setCutoffFrequency(crossoverLM);
    fMidBandChain.get<1>().setCutoffFrequency(crossoverMH);
    fMidBandChain.get<2>().setGainLinear(fBandGainMid);

    fHIghBandChain.get<0>().setCutoffFrequency(crossoverLM);
    fHIghBandChain.get<1>().setCutoffFrequency(crossoverMH);
    fHIghBandChain.get<2>().setGainLinear(fBandGainHigh);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JomsvikingAudioProcessor();
}
