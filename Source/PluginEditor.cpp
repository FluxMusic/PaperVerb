/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PaperVerbAudioProcessorEditor::PaperVerbAudioProcessorEditor (PaperVerbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
preDelaySlider(*audioProcessor.getAPVTS().getParameter("PreDelay")),
sizeSlider(*audioProcessor.getAPVTS().getParameter("Size")),
dampSlider(*audioProcessor.getAPVTS().getParameter("Damping")),
widthSlider(*audioProcessor.getAPVTS().getParameter("Width")),
dryWetSlider(*audioProcessor.getAPVTS().getParameter("WetAmount"))
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    addAndMakeVisible(preDelaySlider);
    addAndMakeVisible(dampSlider);
    addAndMakeVisible(widthSlider);
    addAndMakeVisible(sizeSlider);
    addAndMakeVisible(dryWetSlider);
    
    setSize (540, 280);
}

PaperVerbAudioProcessorEditor::~PaperVerbAudioProcessorEditor()
{
}

//==============================================================================
void PaperVerbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::floralwhite);
    
    const auto fullBounds = getLocalBounds(); //Get rid when finalizing!!
    
    auto bounds = fullBounds;
    
    auto textBounds = bounds.removeFromTop(bounds.getHeight() / 5);

    g.setColour (juce::Colours::black);
    g.setFont (textBounds.getHeight() / 2);
    g.drawFittedText ("PaperVerb", textBounds, juce::Justification::centred, 1);
}

void PaperVerbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const auto fullBounds = getLocalBounds();
    
    auto bounds = fullBounds;
    bounds.removeFromTop(bounds.getHeight() / 5);
    bounds.removeFromLeft(fullBounds.getWidth() / 23);
    bounds.removeFromRight(fullBounds.getWidth() / 23);
    
    auto dryWetBounds = bounds.removeFromBottom(bounds.getHeight() / 4);
    dryWetSlider.setBounds(dryWetBounds);
    
    auto preDelayBounds = bounds.removeFromLeft(bounds.getWidth() / 4);
    auto sizeBounds = bounds.removeFromLeft(bounds.getWidth() / 3);
    auto dampBounds = bounds.removeFromLeft(bounds.getWidth() / 2);
    
    preDelaySlider.setBounds(preDelayBounds);
    sizeSlider.setBounds(sizeBounds);
    dampSlider.setBounds(dampBounds);
    widthSlider.setBounds(bounds);
}
