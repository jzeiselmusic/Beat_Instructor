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