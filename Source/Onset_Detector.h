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
    Onset_Detector(int size, int cooldown = 20, int threshold_length = 100) {
        STFT_size = size;
        onsetSTFT = new STFT(256);
        COOLDOWN_AMOUNT = cooldown;
        THRESHOLD_LENGTH = threshold_length;
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
        if (onsetBuffer.size() > THRESHOLD_LENGTH) {
            /* keep list to maximum 100 */
            onsetBuffer.erase(onsetBuffer.begin());
        }
        
        onsetSTFT->updatePastFFT();
        
        return sum;
    }
    
    bool detectPeakInOnsetBuffer() {
        if (onsetBuffer.size() < THRESHOLD_LENGTH) {
            return false;
        }
        else
        {
            /* calculate average and std of first N values,
               then calculate how much current sample deviates
               from the average */
            if (TRIGGERED) {
                onsetCoolDown += 1;
                if (onsetCoolDown == COOLDOWN_AMOUNT) {
                    onsetCoolDown = 0;
                    TRIGGERED = false;
                }
            }
            std::vector<double> firstN;
            std::copy(onsetBuffer.begin(), onsetBuffer.begin() + THRESHOLD_LENGTH - 5,
                      std::back_inserter(firstN));
            double sum = std::accumulate(std::begin(firstN), std::end(firstN), 0.0);
            double m =  sum / firstN.size();

            double accum = 0.0;
            std::for_each (std::begin(firstN), std::end(firstN), [&](const double d) {
                accum += (d - m) * (d - m);
            });

            double stdev = sqrt(accum / (firstN.size()-1));
            
            onsetMean = m;
            onsetStdev = stdev;
            latestOnset = onsetBuffer.at(THRESHOLD_LENGTH-1);
            
            if (latestOnset > (m + 2*stdev) && latestOnset > 3.0) {
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
    
    float getLatestOnset() {
        return latestOnset;
    }
    
private:
    int STFT_size;
    std::vector<float> onsetBuffer;
    STFT* onsetSTFT;
    float onsetMean = 0.0;
    float onsetStdev = 0.0;
    float latestOnset = 0.0;
    
    int COOLDOWN_AMOUNT;
    int THRESHOLD_LENGTH;
    bool TRIGGERED = false;
    int onsetCoolDown = 0;
};
