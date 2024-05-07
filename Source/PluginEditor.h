/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/PaperKnob.h"
#include "Components/PaperSlider.h"

//==============================================================================
/**
*/
class PaperVerbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PaperVerbAudioProcessorEditor (PaperVerbAudioProcessor&);
    ~PaperVerbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PaperVerbAudioProcessor& audioProcessor;
    
    PaperKnob preDelaySlider, sizeSlider, dampSlider, widthSlider;
    
    PaperSlider dryWetSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaperVerbAudioProcessorEditor)
};
