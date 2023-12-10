/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
Beat_InstructorAudioProcessorEditor::Beat_InstructorAudioProcessorEditor (Beat_InstructorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    startTimerHz(5);
}

Beat_InstructorAudioProcessorEditor::~Beat_InstructorAudioProcessorEditor()
{
}

//==============================================================================
void Beat_InstructorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    g.drawFittedText (positionText, getLocalBounds().reduced(50, 1), juce::Justification::right, 1);
    g.drawFittedText(positionTextTwo, getLocalBounds().reduced(50, 1), juce::Justification::left, 1);
}

void Beat_InstructorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void Beat_InstructorAudioProcessorEditor::timerCallback() {
    auto position = audioProcessor.retrievePositionInfo();
    positionText = juce::String(get<0>(position));
    positionTextTwo = juce::String(get<1>(position));
    //if (audioProcessor.isSpectrumAvailable()) {
        //positionTextTwo = juce::String(audioProcessor.getOnsetSum());
    //}
    repaint();
}
