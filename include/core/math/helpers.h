#pragma once
#include <cmath>
#include <random>
#include <thread>
#include <iostream>

inline int randomInt(int min, int max)
{
    thread_local std::mt19937 rng(
        std::random_device{}() ^
        std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

inline float fRand(float fMin, float fMax)
{
    float f = (float)randomInt(0, RAND_MAX) / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

inline float fRand(float fMax)
{
    float f = (float)randomInt(0, RAND_MAX) / RAND_MAX;
    return f * fMax;
}