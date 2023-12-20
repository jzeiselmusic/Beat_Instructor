/*
  ==============================================================================

    Onset_Detector.h
    Created: 10 Dec 2023 1:23:07pm
    Author:  Jacob Zeisel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "utils.h"
#include "STFT.h"

class Onset_Detector {
public:
    Onset_Detector(int size, int cooldown = 20) {
        STFT_size = size;
        onsetSTFT = new STFT(256);
        COOLDOWN_AMOUNT = cooldown;
    }
    ~Onset_Detector() { delete onsetSTFT ;}
    
    bool process(double sample) {
        bool ready = onsetSTFT->addInput(sample);
        if (ready) {
            onsetSTFT->processFFT();
            performOnsetFunction();
        }
        return ready;
    }
    
    float performOnsetFunction() {
        /* onset detection function takes the FFT spectrum
           and returns a value for each window */
        float sum = 0.0;
        for (int j = 0; j < (STFT_size / 2); ++j) {
            sum += halfWaveRect(onsetSTFT->getInputBuffer()->at(j)
                        - onsetSTFT->getPreviousBuffer()->at(j));
        }
        
        /* add onset to an onset list */
        onsetBuffer.push_back(sum);
        if (onsetBuffer.size() > 100) {
            /* keep list to maximum 100 */
            onsetBuffer.erase(onsetBuffer.begin());
        }
        
        onsetSTFT->updatePastFFT();
        
        return sum;
    }
    
    bool detectPeakInOnsetBuffer() {
        if (onsetBuffer.size() < 100) {
            return false;
        }
        else
        {
            /* calculate average and std of first 90 values,
               then calculate how much current sample deviates
               from the average */
            if (TRIGGERED) {
                onsetCoolDown += 1;
                if (onsetCoolDown == COOLDOWN_AMOUNT) {
                    onsetCoolDown = 0;
                    TRIGGERED = false;
                }
            }
            std::vector<double> firstNinety;
            std::copy(onsetBuffer.begin(), onsetBuffer.begin() + 90,
                      std::back_inserter(firstNinety));
            double sum = std::accumulate(std::begin(firstNinety), std::end(firstNinety), 0.0);
            double m =  sum / firstNinety.size();

            double accum = 0.0;
            std::for_each (std::begin(firstNinety), std::end(firstNinety), [&](const double d) {
                accum += (d - m) * (d - m);
            });

            double stdev = sqrt(accum / (firstNinety.size()-1));
            
            onsetMean = m;
            onsetStdev = stdev;
            
            if (onsetBuffer.at(99) > (m + 4*stdev)) {
                if (!TRIGGERED) {
                    TRIGGERED = true;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
    }
    
    float getMean() {
        return onsetMean;
    }
    
    float getStdev() {
        return onsetStdev;
    }
    
private:
    int STFT_size;
    std::vector<float> onsetBuffer;
    STFT* onsetSTFT;
    float onsetMean = 0.0;
    float onsetStdev = 0.0;
    
    int COOLDOWN_AMOUNT;
    int onsetCoolDown = 0;
    bool TRIGGERED = false;
};
