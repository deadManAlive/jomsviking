/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JomsvikingAudioProcessorEditor::JomsvikingAudioProcessorEditor (JomsvikingAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 500);

    //band 1 gain slider
    bandOneGainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    bandOneGainSlider.setRange(0.0, 1.0);
    bandOneGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    bandOneGainSlider.setTextValueSuffix(" x");
    bandOneGainSlider.setValue(1.0);

    //band 2 gain slider
    bandTwoGainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    bandTwoGainSlider.setRange(0.0, 1.0);
    bandTwoGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    bandTwoGainSlider.setTextValueSuffix(" x");
    bandTwoGainSlider.setValue(1.0);

    //crossover slider
    crossoverSlider.setSliderStyle(juce::Slider::LinearBar);
    crossoverSlider.setRange(20.0, 20000.0);
    crossoverSlider.setSkewFactorFromMidPoint(500.0);
    crossoverSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    crossoverSlider.setValue(2000.0);

    //add to GUI
    addAndMakeVisible(&bandOneGainSlider);
    addAndMakeVisible(&bandTwoGainSlider);
    addAndMakeVisible(&crossoverSlider);
    
}

JomsvikingAudioProcessorEditor::~JomsvikingAudioProcessorEditor()
{
}

//==============================================================================
void JomsvikingAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour::fromRGB(238, 238, 238));

    g.setColour (juce::Colours::black);
    g.setFont (16.0f);
    g.drawFittedText("Band 1 Gain", 0, 0, getWidth()/2, 20, juce::Justification::centred, 1);
    g.drawFittedText("Band 2 Gain", 40, 0, getWidth() / 2, 20, juce::Justification::centred, 1);
    g.drawFittedText("Crossover", 0, getHeight() - 40, getWidth() - 60, 20, juce::Justification::centred, 1);
}

void JomsvikingAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    bandOneGainSlider.setBounds(40, 30, 20, getHeight() - 120);
    bandTwoGainSlider.setBounds(120, 30, 20, getHeight() - 120);
    crossoverSlider.setBounds(40, getHeight() - 80, getWidth() - 80, 20);
}
