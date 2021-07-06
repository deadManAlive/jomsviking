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

    //sliders
    juce::Slider lcrossSlider{ juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::Slider rcrossSlider{ juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::Slider inGainLowSlider{ juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::Slider inGainMidSlider{ juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::Slider inGainHghSlider{ juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::ComboBox oversamplingDropdown;

    //slider attachment
    juce::AudioProcessorValueTreeState::SliderAttachment lcrossSliderAttachment{ audioProcessor.processTreeState, "lcrossover", lcrossSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment rcrossSliderAttachment{ audioProcessor.processTreeState, "rcrossover", rcrossSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment inGainLowSliderAttachment{ audioProcessor.processTreeState, "low_ingain", inGainLowSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment inGainMidSliderAttachment{ audioProcessor.processTreeState, "mid_ingain", inGainMidSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment inGainHghSliderAttachment{ audioProcessor.processTreeState, "hgh_ingain", inGainHghSlider };
    juce::AudioProcessorValueTreeState::ComboBoxAttachment oversamplingDropdownAttachment{audioProcessor.processTreeState, "oversampling", oversamplingDropdown};

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JomsvikingAudioProcessorEditor)
};
