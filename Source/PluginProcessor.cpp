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
    mSampleRate = sampleRate;

    juce::dsp::ProcessSpec proSpec;
    proSpec.sampleRate = sampleRate;
    proSpec.maximumBlockSize = samplesPerBlock;
    proSpec.numChannels = getMainBusNumOutputChannels();

    updateProcessorChains();

    bandChainLow.prepare(proSpec);
    bandChainMid.prepare(proSpec);
    bandChainHigh.prepare(proSpec);
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    updateProcessorChains();

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


    bandChainLow.process(juce::dsp::ProcessContextReplacing<float>(bandBlockLow));
    bandChainMid.process(juce::dsp::ProcessContextReplacing<float>(bandBlockMid));
    bandChainHigh.process(juce::dsp::ProcessContextReplacing<float>(bandBlockHigh));

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
    return new JomsvikingAudioProcessorEditor (*this);
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

void JomsvikingAudioProcessor::updateProcessorChains() {
    float crossoverLM = pCrossoverLM;
    float crossoverMH = pcrossoverMH;

    float fBandGainLow = pBandGainLow;
    float fBandGainMid = pBandGainMid;
    float fBandGainHigh = pBandGainHigh;

    bandChainLow.get<0>().setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    bandChainLow.get<0>().setCutoffFrequency(crossoverLM);
    bandChainLow.get<1>().setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    bandChainLow.get<1>().setCutoffFrequency(crossoverMH);
    bandChainLow.get<2>().setGainLinear(fBandGainLow);

    bandChainMid.get<0>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    bandChainMid.get<0>().setCutoffFrequency(crossoverLM);
    bandChainMid.get<1>().setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    bandChainMid.get<1>().setCutoffFrequency(crossoverMH);
    bandChainMid.get<2>().setGainLinear(fBandGainMid);

    bandChainHigh.get<0>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    bandChainHigh.get<0>().setCutoffFrequency(crossoverMH);
    bandChainHigh.get<1>().setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    bandChainHigh.get<1>().setCutoffFrequency(crossoverLM);
    bandChainHigh.get<2>().setGainLinear(fBandGainHigh);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JomsvikingAudioProcessor();
}
