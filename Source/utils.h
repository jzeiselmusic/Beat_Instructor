/*
  ==============================================================================

    utils.h
    Created: 10 Dec 2023 2:53:33pm
    Author:  Jacob Zeisel

  ==============================================================================
*/

#pragma once

#include <math.h>

inline float halfWaveRect(float n) {
    return ((n + abs(n)) / 2.0);
}


inline double computeXcorr(int lag, std::vector<double>* X, int size, double mean, double var)
{
    double   autocv;
    double   ac_value;

    // Loop to compute autovariance
    autocv = 0.0;
    
    for (int i = 0; i < (size - lag); i++) {
        autocv = autocv + ((X->at(i) - mean) * (X->at(i+lag) - mean));
    }
    autocv *= (1.0 / (size - lag));

    // Autocorrelation is autocovariance divided by variance
    ac_value = autocv / var;

    return(ac_value);
}
