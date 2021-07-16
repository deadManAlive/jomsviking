/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
JomsvikingAudioProcessorEditor::JomsvikingAudioProcessorEditor (JomsvikingAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    //oversampling
    oversamplingDropdown.addItem("No Oversampling", 1);
    for (int i = 2; i <= 7; i++) {
        juce::String opt;
        opt << pow(2, i - 1);
        opt << "x";
        oversamplingDropdown.addItem(opt, i);
    }
    oversamplingDropdown.setSelectedId(1);

    for (auto* component : getComponents()) {
        addAndMakeVisible(component);
    }

    setSize (1024, 512);

    sectorizationBGImage = juce::ImageCache::getFromMemory(BinaryData::Jomsviking_Layout_png, BinaryData::Jomsviking_Layout_pngSize);
    
    setResizable(false, false);

    
}

JomsvikingAudioProcessorEditor::~JomsvikingAudioProcessorEditor()
{
}

//==============================================================================
void JomsvikingAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (juce::Colour::fromRGBA(10, 10, 10, 255));
    g.drawImageAt(sectorizationBGImage, 0, 0);

    g.setColour (juce::Colours::black);
    g.setFont (16.0f);
}

void JomsvikingAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    inGainLowSlider.setBounds(192, 256, 105, 128);
    inGainMidSlider.setBounds(459, 256, 105, 128);
    inGainHghSlider.setBounds(727, 256, 105, 128);

    lcrossSlider.setBounds(352, 256, 107, 128);
    rcrossSlider.setBounds(564, 256, 107, 128);

    oversamplingDropdown.setBounds(0, 256, 192, 64);
}

void JomsvikingAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    
}

std::vector<juce::Component*> JomsvikingAudioProcessorEditor::getComponents() {
    return {
        &lcrossSlider,
        &rcrossSlider,
        &inGainLowSlider,
        &inGainMidSlider,
        &inGainHghSlider,
        &oversamplingDropdown
    };
}


