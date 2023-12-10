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
        OD_size = size;
        binsMagsNew = (float*)malloc((size_t)size);
        binsMagsOld = (float*)malloc((size_t)size);
        stft = gam::STFT(size, size/4, 0, 
                gam::WindowType::HAMMING, 
                gam::SpectralType::COMPLEX);
    }

    ~Onset_Detector() {
        free(binsMagsNew);
        free(binsMagsOld);
    }

    /* to use this class: process every sample */
    /* if return true when processing sample, then grab the latest avail sum */

    bool process(double input) {
        isSpectrumReady = stft((float)input);

        if (isSpectrumReady) {
            for (int i = 0; i < stft.numBins(); ++i) {
                // store each bin as the L1 norm of the complex FFT
                binsMagsOld[i] = binsMagsNew[i];
                binsMagsNew[i] = abs(stft.bin(i).real()) + abs(stft.bin(i).imag());
            }
        }
        return isSpectrumReady;
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

private:
    bool isSpectrumReady;
    int OD_size;
    double OD_sampleRate;
    gam::STFT stft;
    float* binsMagsNew;
    float* binsMagsOld;
};
