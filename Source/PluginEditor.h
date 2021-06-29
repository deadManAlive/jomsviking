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

    juce::Slider bandGainSliderLow;
    juce::Label bandGainLabelLow;

    juce::Slider bandGainSliderMid;
    juce::Label bandGainLabelMid;

    juce::Slider bandGainSliderHigh;
    juce::Label bandGainLabelHigh;
    
    juce::Slider crossoverSliderLM;
    juce::Label crossoverLabelLM;

    juce::Slider crossoverSliderMH;
    juce::Label crossoverLabelMH;

    juce::Image sectorizationBGImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessorEditor)
};
