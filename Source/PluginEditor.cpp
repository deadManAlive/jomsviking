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

    //low gain slider
    bandGainSliderLow.setSliderStyle(juce::Slider::Rotary);
    bandGainSliderLow.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bandGainSliderLow.setRange(0.0, 2.0);
    bandGainSliderLow.setTextValueSuffix(" x");
    bandGainSliderLow.addListener(this);
    bandGainSliderLow.setValue(1.0);

    //bandGainLabelLow.setText("Low Gain", juce::dontSendNotification);
    //bandGainLabelLow.attachToComponent(&bandGainSliderLow, false);
    //bandGainLabelLow.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //mid gain slider
    bandGainSliderMid.setSliderStyle(juce::Slider::Rotary);
    bandGainSliderMid.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bandGainSliderMid.setRange(0.0, 2.0);
    bandGainSliderMid.setTextValueSuffix(" x");
    bandGainSliderMid.addListener(this);
    bandGainSliderMid.setValue(1.0);

    //bandGainLabelMid.setText("Mid Gain", juce::dontSendNotification);
    //bandGainLabelMid.attachToComponent(&bandGainSliderMid, false);
    //bandGainLabelMid.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //high gain slider
    bandGainSliderHigh.setSliderStyle(juce::Slider::Rotary);
    bandGainSliderHigh.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bandGainSliderHigh.setRange(0.0, 2.0);
    bandGainSliderHigh.setTextValueSuffix(" x");
    bandGainSliderHigh.addListener(this);
    bandGainSliderHigh.setValue(1.0);

    //bandGainLabelHigh.setText("High Gain", juce::dontSendNotification);
    //bandGainLabelHigh.attachToComponent(&bandGainSliderHigh, false);
    //bandGainLabelHigh.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //L-M crossover slider
    crossoverSliderLM.setSliderStyle(juce::Slider::LinearBar);
    crossoverSliderLM.setRange(20.0, 1000.0);
    crossoverSliderLM.setSkewFactorFromMidPoint(500.0);
    crossoverSliderLM.setTextValueSuffix(" Hz");
    crossoverSliderLM.addListener(this);
    crossoverSliderLM.setValue(50.0);

    //crossoverLabelLM.setText("Low-Mid Crossover Frequency", juce::dontSendNotification);
    //crossoverLabelLM.attachToComponent(&crossoverSliderLM, false);
    //crossoverLabelLM.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //M-H crossover slider
    crossoverSliderMH.setSliderStyle(juce::Slider::LinearBar);
    crossoverSliderMH.setRange(50.0, 20000.0);
    crossoverSliderMH.setSkewFactorFromMidPoint(500.0);
    crossoverSliderMH.setTextValueSuffix(" Hz");
    crossoverSliderMH.addListener(this);
    crossoverSliderMH.setValue(1000.0);

    //crossoverLabelMH.setText("Mid-High Crossover Frequency", juce::dontSendNotification);
    //crossoverLabelMH.attachToComponent(&crossoverSliderLM, false);
    //crossoverLabelMH.setColour(juce::Label::textColourId, juce::Colour(0, 0, 0));

    //add to GUI
    addAndMakeVisible(&bandGainSliderLow);
    //addAndMakeVisible(&bandGainLabelLow);

    addAndMakeVisible(&bandGainSliderMid);
    //addAndMakeVisible(&bandGainLabelMid);

    addAndMakeVisible(&bandGainSliderHigh);
    //addAndMakeVisible(&bandGainSliderHigh);

    addAndMakeVisible(&crossoverSliderLM);
    //addAndMakeVisible(&crossoverLabelLM);

    addAndMakeVisible(&crossoverSliderMH);
    //addAndMakeVisible(&crossoverLabelMH);
    
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
    bandGainSliderLow.setBounds(192, 256, 105, 128);
    bandGainSliderMid.setBounds(459, 256, 105, 128);
    bandGainSliderHigh.setBounds(727, 256, 105, 128);

    crossoverSliderLM.setBounds(352, 256, 107, 128);
    crossoverSliderMH.setBounds(564, 256, 107, 128);
}

void JomsvikingAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    audioProcessor.pBandGainLow = bandGainSliderLow.getValue();
    audioProcessor.pBandGainMid = bandGainSliderMid.getValue();
    audioProcessor.pBandGainHigh = bandGainSliderHigh.getValue();

    audioProcessor.pCrossoverLM = crossoverSliderLM.getValue();
    audioProcessor.pcrossoverMH = crossoverSliderMH.getValue();

    crossoverSliderLM.setRange(20.0, crossoverSliderMH.getValue() < 3000.0?crossoverSliderMH.getValue():3000.0);
    crossoverSliderMH.setRange(crossoverSliderLM.getValue() > 300?crossoverSliderLM.getValue():300.0, 16000.0);
}
