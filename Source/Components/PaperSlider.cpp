/*
  ==============================================================================

    PaperSlider.cpp
    Created: 6 May 2024 12:44:57pm
    Author:  Alex Huber

  ==============================================================================
*/

#include "PaperSlider.h"

PaperSlider::PaperSlider(juce::RangedAudioParameter& rap)
: juce::Slider(juce::Slider::SliderStyle::LinearBar, juce::Slider::TextEntryBoxPosition::NoTextBox),
param(&rap)
{
    setLookAndFeel(&lnf);
    setSliderSnapsToMousePosition(false);
}

PaperSlider::~PaperSlider()
{
    setLookAndFeel(nullptr);
    
//    param = nullptr;
}

void PaperSlider::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    auto wetBounds = bounds.removeFromTop(bounds.getHeight() / 3);
    auto dryBounds = wetBounds.removeFromLeft(wetBounds.getWidth() / 2);
    
    const auto minSliderPos = static_cast<double>(bounds.getX() + bounds.getWidth() / 19);
    const auto maxSliderPos = static_cast<double>(bounds.getWidth() - bounds.getWidth() / 19);
    
    const auto range = getRange();
    
    getLookAndFeel().drawLinearSlider(g,
                                      bounds.getX(),
                                      bounds.getY(),
                                      bounds.getWidth(),
                                      bounds.getHeight(),
                                      juce::jmap(getValue(), range.getStart(), range.getEnd(), minSliderPos, maxSliderPos),
                                      minSliderPos,
                                      maxSliderPos,
                                      juce::Slider::SliderStyle::LinearBar,
                                      *this);
    
    g.setColour(juce::Colours::black);
    g.setFont(wetBounds.getHeight());
    g.drawFittedText("Dry", dryBounds, juce::Justification::centred, 1);
    g.drawFittedText("Wet", wetBounds, juce::Justification::centred, 1);
}
