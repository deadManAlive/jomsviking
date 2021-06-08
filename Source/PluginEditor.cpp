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
    setSize (1024, 512);

    sectorizationBGImage = juce::ImageCache::getFromMemory(BinaryData::Jomsviking_Layout_png, BinaryData::Jomsviking_Layout_pngSize);

    //band 1 gain slider
    bandOneGainSlider.setSliderStyle(juce::Slider::Rotary);
    bandOneGainSlider.setRange(0.0, 2.0);
    //bandOneGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    bandOneGainSlider.setTextValueSuffix(" x");
    bandOneGainSlider.addListener(this);
    bandOneGainSlider.setValue(1.0);

    bandOneGainLabel.setText("Band 1 Gain", juce::dontSendNotification);
    bandOneGainLabel.attachToComponent(&bandOneGainSlider, true);
    bandOneGainLabel.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //band 2 gain slider
    bandTwoGainSlider.setSliderStyle(juce::Slider::Rotary);
    bandTwoGainSlider.setRange(0.0, 2.0);
    //bandTwoGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    bandTwoGainSlider.setTextValueSuffix(" x");
    bandTwoGainSlider.addListener(this);
    bandTwoGainSlider.setValue(1.0);

    bandTwoGainLabel.setText("Band 2 Gain", juce::dontSendNotification);
    bandTwoGainLabel.attachToComponent(&bandTwoGainSlider, true);
    bandTwoGainLabel.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //crossover slider
    crossoverSlider.setSliderStyle(juce::Slider::LinearBar);
    crossoverSlider.setRange(20.0, 20000.0);
    crossoverSlider.setSkewFactorFromMidPoint(500.0);
    //crossoverSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    crossoverSlider.setTextValueSuffix(" Hz");
    crossoverSlider.addListener(this);
    crossoverSlider.setValue(2000.0);

    crossoverLabel.setText("Crossover Frequency", juce::dontSendNotification);
    crossoverLabel.attachToComponent(&crossoverSlider, true);
    crossoverLabel.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //add to GUI
    addAndMakeVisible(&bandOneGainSlider);
    addAndMakeVisible(&bandOneGainLabel);
    addAndMakeVisible(&bandTwoGainSlider);
    addAndMakeVisible(&bandTwoGainLabel);
    addAndMakeVisible(&crossoverSlider);
    addAndMakeVisible(&crossoverLabel);
    
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
    bandOneGainSlider.setBounds(192, 256, 160, 128);
    bandTwoGainSlider.setBounds(671, 256, 160, 128);
    crossoverSlider.setBounds(459, 256, 105, 128);
}

void JomsvikingAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    audioProcessor.pBandOneGain = bandOneGainSlider.getValue();
    audioProcessor.pBandTwoGain = bandTwoGainSlider.getValue();
    audioProcessor.pCrossover = crossoverSlider.getValue();
}
