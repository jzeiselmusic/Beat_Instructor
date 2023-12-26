/*
  ==============================================================================

    Tempo_Estimator.h
    Created: 9 Dec 2023 7:30:22pm
    Author:  Jacob Zeisel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Onset_Detector.h"

using namespace juce;

class Tempo_Estimator {
public:
    Tempo_Estimator(int window_size) {
        windowSize = window_size;
        tempoEstimatorOnsets = new std::vector<double>(windowSize);
    }
    
    ~Tempo_Estimator() {
        delete tempoEstimatorOnsets;
    }
    
    bool process(double onset) {
        tempoEstimatorOnsets->push_back(onset);
        if (tempoEstimatorOnsets->size() == windowSize) {
            return true;
        }
        else {
            return false;
        }
    }
    
    void processTempoCalculation() {
        if (tempoEstimatorOnsets->size() != windowSize) {
            throw std::runtime_error("error: onset buffer is not full");
            return;
        }
        
        double sum = std::accumulate(std::begin(*tempoEstimatorOnsets),
                                     std::end(*tempoEstimatorOnsets), 0.0);
        double m =  sum / tempoEstimatorOnsets->size();
        double accum = 0.0;
        std::for_each (std::begin(*tempoEstimatorOnsets),
                       std::end(*tempoEstimatorOnsets), [&](const double d) {
            accum += (d - m) * (d - m);
        });

        double var = accum / (tempoEstimatorOnsets->size()-1);
        
        std::vector<double> xcorrs;
        
        // do xcorr for each lag that we are interested in (every hundredth?)
        // these calculations are not correct right now
        // every onset data point is for every 256th sample (FFT buffer)
        for (int lag = 100; lag < 44100; lag += 100) {
            xcorrs.push_back(computeXcorr(lag, tempoEstimatorOnsets, (int)tempoEstimatorOnsets->size(), m, var));
        }
        
        auto element = std::max_element(std::begin(xcorrs), std::end(xcorrs));
        
        int index = (int)std::distance(std::begin(xcorrs), element);
        int sampleOffset = index*100;
        currentTempo = (44100 / sampleOffset) * 60;
    }
        
private:
    double currentTempo = 120.0;
    int windowSize;
    std::vector<double>* tempoEstimatorOnsets;
};
