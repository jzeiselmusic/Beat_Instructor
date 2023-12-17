/*
  ==============================================================================

    STFT.h
    Created: 17 Dec 2023 1:58:24pm
    Author:  Jacob Zeisel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "utils.h"

class STFT {
public:
    STFT(double sample_rate, int size) {
        
        if ((size & size-1) != 0) throw
            std::runtime_error("size of fft must be power of 2");
        STFT_size = size;
        STFT_sampleRate = sample_rate;
        
        /* allocate space for input and fft */
        STFT_inputBuffer = new std::vector<float>(size, 0.0);
        STFT_previousFFTBuffer = new std::vector<float>(size, 0.0);
        STFT_fftModule = new juce::dsp::FFT(log2((double)size));
    }
    
    ~STFT() {
        delete STFT_inputBuffer;
        delete STFT_previousFFTBuffer;
        delete STFT_fftModule;
    }
    
    bool addInput(double n) {
        STFT_inputBuffer->at(STFT_inputIndex) = n;
        STFT_inputIndex += 1;
        if (STFT_inputIndex == STFT_size) {
            STFT_inputIndex = 0;
            STFT_isInputReadyToBeProcessed = true;
            return true;
        }
        else {
            return false;
        }
    }
    
    std::tuple<std::vector<float>, bool> processFFT() {
        /* when input vector is full, process fft into fft buffer
           and then return the result to the user.
         
           return: tuple with copy of fft buffer and boolean to
                   show whether process was correct or not
         */
        if (STFT_inputIndex != 0 &&
            STFT_isInputReadyToBeProcessed != true) {
            return {*STFT_inputBuffer, false};
        }
        else {
            /* calculates both positive and negative frequencies */
            /* usable values will be half of STFT_size */
            STFT_fftModule->performFrequencyOnlyForwardTransform
            (&(STFT_inputBuffer->at(0)), false);
            
            STFT_isInputReadyToBeProcessed = false;
            STFT_isFFTReady = true;
            
            return {*STFT_inputBuffer, true};
        }
    }
    
    float performOnsetFunction() {
        if (STFT_isFFTReady == false) throw std::runtime_error("FFT not ready");
        
        /* onset detection function takes the FFT spectrum
           and returns a value for each window */
        float sum = 0.0;
        for (int j = 0; j < (STFT_size / 2); ++j) {
            sum += halfWaveRect(STFT_inputBuffer->at(j)
                        - STFT_previousFFTBuffer->at(j));
        }
        
        updatePastFFT();
        
        return sum;
    }
    
    void updatePastFFT() {
        *STFT_previousFFTBuffer = *STFT_inputBuffer;
    }
    
    
private:
    bool STFT_isFFTReady = false;
    bool STFT_isInputReadyToBeProcessed = false;
    int STFT_size;
    double STFT_sampleRate;
    
    int STFT_inputIndex = 0;
    
    std::vector<float>* STFT_inputBuffer;
    std::vector<float>* STFT_previousFFTBuffer;
    juce::dsp::FFT* STFT_fftModule;
};
