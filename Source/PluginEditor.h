/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Metronome_Figure.h"

//==============================================================================
/**
*/
class Beat_InstructorAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Beat_InstructorAudioProcessorEditor (Beat_InstructorAudioProcessor&);
    ~Beat_InstructorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Beat_InstructorAudioProcessor& audioProcessor;
    
    juce::String positionText = "0";
    juce::String positionTextTwo = "0";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Beat_InstructorAudioProcessorEditor)
};
