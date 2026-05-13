#pragma once
#include <cmath>

inline float fRand(float fMin, float fMax)
{
    float f = (float)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

inline float fRand(float fMax)
{
    float f = (float)rand() / RAND_MAX;
    return f * fMax;
}