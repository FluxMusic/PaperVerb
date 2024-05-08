/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PaperVerbAudioProcessor::PaperVerbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

PaperVerbAudioProcessor::~PaperVerbAudioProcessor()
{
}

//==============================================================================
const juce::String PaperVerbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PaperVerbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PaperVerbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PaperVerbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PaperVerbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PaperVerbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PaperVerbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PaperVerbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PaperVerbAudioProcessor::getProgramName (int index)
{
    return {};
}

void PaperVerbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PaperVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();
    spec.maximumBlockSize = samplesPerBlock;
    
    auto parameters = updateReverbParameters();
    reverb.setParameters(parameters);
    
    preDelay.setMaximumDelayInSamples(sampleRate);
    
    preDelay.prepare(spec);
    
    reverb.prepare(spec);
    
    dryWetMixer.prepare(spec);
}

void PaperVerbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PaperVerbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PaperVerbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    preDelay.setDelay(calculateTimeToSamples(apvts.getRawParameterValue("PreDelay")->load()));
    
    auto parameters = updateReverbParameters();
    reverb.setParameters(parameters);
    
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    juce::dsp::AudioBlock<float> dryBlock(buffer);
    
    dryWetMixer.pushDrySamples(dryBlock);
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    preDelay.process(context);
    
    reverb.process(context);
    
    dryWetMixer.setWetMixProportion(apvts.getRawParameterValue("WetAmount")->load());
    
    dryWetMixer.mixWetSamples(block);
}

//==============================================================================
bool PaperVerbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PaperVerbAudioProcessor::createEditor()
{
    return new PaperVerbAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void PaperVerbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PaperVerbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PaperVerbAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout PaperVerbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    auto percentageRange = juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f);
    auto preDelayRange = juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f);
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Size", 1), "Size", percentageRange, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Damping", 2), "Damping", percentageRange, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Width", 3), "Width", percentageRange, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("WetAmount", 4), "Wet Amount", percentageRange, 1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PreDelay", 5), "Pre Delay", preDelayRange, 10.f));
    
    return layout;
}

juce::Reverb::Parameters PaperVerbAudioProcessor::updateReverbParameters()
{
    juce::Reverb::Parameters parameters;
    
    parameters.roomSize = apvts.getRawParameterValue("Size")->load();
    parameters.damping = apvts.getRawParameterValue("Damping")->load();
    parameters.width = apvts.getRawParameterValue("Width")->load();
    
    parameters.wetLevel = apvts.getRawParameterValue("WetAmount")->load();
    parameters.dryLevel = 1 - parameters.wetLevel;
    
    return parameters;
}

float PaperVerbAudioProcessor::calculateTimeToSamples(float delayInMilliseconds)
{
    const auto sampleRate = getSampleRate();
    
    return static_cast<float>(delayInMilliseconds * (sampleRate / 1000));
}
