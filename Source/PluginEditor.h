/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class JomsvikingAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    JomsvikingAudioProcessorEditor (JomsvikingAudioProcessor&);
    ~JomsvikingAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JomsvikingAudioProcessor& audioProcessor;

    juce::Slider bandOneGainSlider;
    juce::Label bandOneGainLabel;

    juce::Slider bandTwoGainSlider;
    juce::Label bandTwoGainLabel;
    
    juce::Slider crossoverSlider;
    juce::Label crossoverLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessorEditor)
};
