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
struct CustomRotary : juce::Slider {
    CustomRotary() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow) {

    }
};

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

    juce::Image sectorizationBGImage;

    CustomRotary lcrossSlider, rcrossSlider, inGainLowSlider, inGainMidSlider, inGainHghSlider;
    juce::ComboBox oversamplingDropdown;

    juce::AudioProcessorValueTreeState::SliderAttachment lcrossSliderAttachment, rcrossSliderAttachment, inGainLowSliderAttachment, inGainMidSliderAttachment, inGainHghSliderAttachment;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment oversamplingDropdownAttachment;

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessorEditor)
};
