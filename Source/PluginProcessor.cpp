/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace std;

//==============================================================================
Beat_InstructorAudioProcessor::Beat_InstructorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Beat_InstructorAudioProcessor::~Beat_InstructorAudioProcessor()
{
}

//==============================================================================
const juce::String Beat_InstructorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Beat_InstructorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Beat_InstructorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Beat_InstructorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Beat_InstructorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Beat_InstructorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Beat_InstructorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Beat_InstructorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Beat_InstructorAudioProcessor::getProgramName (int index)
{
    return {};
}

void Beat_InstructorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Beat_InstructorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Beat_InstructorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Beat_InstructorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Beat_InstructorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    metronomeAudio.updateLocationOfBuffer();
    getCurrentPosition();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        if (channel == 0) {
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                // onsetDetector.process(channelData[i]);
            }
        }
    }
}

//==============================================================================

bool Beat_InstructorAudioProcessor::isSpectrumAvailable() {
    return onsetDetector.isSpectrumAvailable();
}

float Beat_InstructorAudioProcessor::getOnsetSum() {
    return onsetDetector.onsetDectionSum();
}

void Beat_InstructorAudioProcessor::getCurrentPosition()
{
    auto currentPos = metronomeAudio.getLocationOfBuffer();
    
    ppqPosition = get<0>(currentPos);
    ppqPositionOfLastBarStart = get<1>(currentPos);
}

tuple<double, double> Beat_InstructorAudioProcessor::retrievePositionInfo()
{
    return {ppqPosition, ppqPositionOfLastBarStart};
}

bool Beat_InstructorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Beat_InstructorAudioProcessor::createEditor()
{
    return new Beat_InstructorAudioProcessorEditor (*this);
}

//==============================================================================
void Beat_InstructorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Beat_InstructorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Beat_InstructorAudioProcessor();
}
