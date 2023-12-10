/*
  ==============================================================================

    Metronome_Audio.h
    Created: 9 Dec 2023 4:52:34pm
    Author:  Jacob Zeisel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

using namespace juce;
using namespace std;

class Metronome_Audio {
public:
    Metronome_Audio(AudioProcessor* plugin_processor, int sample_rate) {
        MA_pluginProcessor = plugin_processor;
        MA_sampleRate = sample_rate;
    }
    
    void updateLocationOfBuffer() {
        MA_audioPlayHead = MA_pluginProcessor->getPlayHead();
        Optional<AudioPlayHead::PositionInfo> position = MA_audioPlayHead->getPosition();
        
        if (position.hasValue()) {
            Optional<double> ppqPos = position->getPpqPosition();
            Optional<double> barPpqPos = position->getPpqPositionOfLastBarStart();
            
            if (ppqPos.hasValue() && barPpqPos.hasValue()) {
                MA_ppqPos = *ppqPos;
                MA_barPpqPos = *barPpqPos;
            }
        }
    }
    
    tuple<double, double> getLocationOfBuffer() {
        return {MA_ppqPos, MA_barPpqPos};
    }
    
private:
    AudioProcessor* MA_pluginProcessor;
    AudioPlayHead* MA_audioPlayHead;
    int MA_sampleRate;
    double MA_currentBpm;
    
    AudioPlayHead::TimeSignature MA_currentTimeSignature;
    
    double MA_ppqPos = 0.0;
    double MA_barPpqPos = 0.0;
};
                    
