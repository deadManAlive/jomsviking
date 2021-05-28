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
    ), tree(*this, nullptr, juce::Identifier("Parameters"),
        {
            std::make_unique<juce::AudioParameterFloat>("crossover", "crossover", 20.0, 20000.0, 2000.0),
            std::make_unique<juce::AudioParameterFloat>("bandOneGain", "bandOneGain", 0.0f, 2.0f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("bandTwoGain", "bandTwoGain", 0.0f, 2.0f, 1.0f)
        })
#endif
{
    pCrossover = tree.getRawParameterValue("crossover");
    pBandOneGain = tree.getRawParameterValue("bandOneGain");
    pBandTwoGain = tree.getRawParameterValue("bandTwoGain");
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

    bandOneChain.prepare(proSpec);
    bandTwoChain.prepare(proSpec);
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateProcessorChains();

    juce::AudioBuffer<float> bandOneBuffer;
    juce::AudioBuffer<float> bandTwoBuffer;
    bandOneBuffer.setSize(totalNumInputChannels, bufferLength);
    bandTwoBuffer.setSize(totalNumInputChannels, bufferLength);

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        auto* bufferData = buffer.getReadPointer(channel);

        bandOneBuffer.copyFrom(channel, 0, bufferData, bufferLength);
        bandTwoBuffer.copyFrom(channel, 0, bufferData, bufferLength);
    }

    juce::dsp::AudioBlock<float> bandOneBlock(bandOneBuffer);
    juce::dsp::AudioBlock<float> bandTwoBlock(bandTwoBuffer);
    bandOneChain.process(juce::dsp::ProcessContextReplacing<float>(bandOneBlock));
    bandTwoChain.process(juce::dsp::ProcessContextReplacing<float>(bandTwoBlock));

    buffer.clear();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* bandOneData = bandOneBuffer.getReadPointer(channel);
        auto* bandTwoData = bandTwoBuffer.getReadPointer(channel);
        buffer.addFrom(channel, 0, bandOneData, bufferLength);
        buffer.addFrom(channel, 0, bandTwoData, bufferLength);
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
    float crossOver = *pCrossover;
    float bandonegain = *pBandOneGain;
    float bandtwogain = *pBandTwoGain;

    bandOneChain.get<0>().state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
    bandOneChain.get<0>().state->setCutOffFrequency(mSampleRate, crossOver, (1.0 / juce::MathConstants<double>::sqrt2));
    bandOneChain.get<1>().setGainLinear(bandOneGain);

    bandTwoChain.get<0>().state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
    bandTwoChain.get<0>().state->setCutOffFrequency(mSampleRate, crossOver, (1.0 / juce::MathConstants<double>::sqrt2));
    bandTwoChain.get<1>().setGainLinear(bandTwoGain);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JomsvikingAudioProcessor();
}
