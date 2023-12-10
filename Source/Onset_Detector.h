/*
  ==============================================================================

    Onset_Detector.h
    Created: 10 Dec 2023 1:23:07pm
    Author:  Jacob Zeisel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Gamma/Gamma/DFT.h"
#include "utils.h"

class Onset_Detector {
public:
    Onset_Detector(double sample_rate, int size) {
        OD_sampleRate = sample_rate;
        stft = gam::STFT(size, size/4, 0, 
                gam::WindowType::HAMMING, 
                gam::SpectralType::COMPLEX);
        
        OD_isSpectrumAvailable = false;
        binsMagsNew = (float*)malloc(sizeof(stft.numBins()));
        binsMagsOld = (float*)malloc(sizeof(stft.numBins()));
        OD_size = stft.numBins();
    }

    ~Onset_Detector() {
        free(binsMagsNew);
        free(binsMagsOld);
    }

    /* to use this class: process every sample */
    /* if return true when processing sample, then grab the latest avail sum */

    bool process(float input) {
        OD_isSpectrumReady = stft(input);

        if (OD_isSpectrumReady) {
            OD_isSpectrumAvailable = true;
            for (int i = 0; i < stft.numBins(); ++i) {
                // store each bin as the L1 norm of the complex FFT
                binsMagsOld[i] = binsMagsNew[i];
                binsMagsNew[i] = abs(stft.bin(i).real()) + abs(stft.bin(i).imag());
            }
        }
        return OD_isSpectrumReady;
    }

    float* getLatestSpectrum() {
        return binsMagsNew;
    }

    float onsetDectionSum() {
        float total = 0.0;
        for (int i = 0; i < stft.numBins(); ++i) {
            total += halfWaveRect(binsMagsNew[i] - binsMagsOld[i]);
        }
        return total;
    }
    
    bool isSpectrumAvailable() {
        return OD_isSpectrumAvailable;
    }

private:
    gam::STFT stft;
    
    bool OD_isSpectrumReady;
    bool OD_isSpectrumAvailable;
    int OD_size;
    double OD_sampleRate;
    float* binsMagsNew;
    float* binsMagsOld;
};
