#include "engine.h"
#include <stdio.h>
#include <iostream>

// Iterations
constexpr int iterations = 1'000'000'000; // 1kkk

// Prevent optimization
template <typename T>
inline void doNotOptimize(T const &value)
{
    asm volatile("" : : "g"(value) : "memory");
}

void benchDotVec2();
void benchDotVec3();
void benchDotVec4();
void benchCross();

int main()
{
    std::cout << "Math tests" << std::endl;
    wne::Vector3 v3a(4.0f, 2.0f, 1.0f);
    wne::Vector3 v3b(5.0f, 1.0f, 0.0f);
    wne::Vector3 v3c(2.0f, -1.0f, -1.0f);

    // Dot benches
    benchDotVec2();
    benchDotVec3();
    benchDotVec4();
    benchCross();

    return 0;
}

void benchDotVec2()
{
    wne::Vector2 va(1.0f, 2.0f);
    wne::Vector2 vb(2.0f, 1.0f);

    std::cout << "Vec2 dot bench" << std::endl;
    {
        float result = 0.0f;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i)
        {
            va.x += 0.000001f;
            vb.y -= 0.000001f;
            result += wne::dot(va, vb);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "wne dot: " << diff.count() << " s\n";
    }
}

void benchDotVec3()
{
    wne::Vector3 va(1.0f, 2.0f, 1.0f);
    wne::Vector3 vb(2.0f, 1.0f, 0.0f);

    std::cout << "Vec3 dot bench" << std::endl;
    {
        float result = 0.0f;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i)
        {
            va.x += 0.000001f;
            vb.y -= 0.000001f;
            va.z += 0.000001f;
            result += wne::dot(va, vb);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "wne dot: " << diff.count() << " s\n";
    }
}

void benchDotVec4()
{
    wne::Vector4 va(1.0f, 2.0f, 1.0f, 4.0f);
    wne::Vector4 vb(2.0f, 1.0f, 0.0f, -2.0f);

    std::cout << "Vec4 dot bench" << std::endl;
    {
        float result = 0.0f;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i)
        {
            va.x += 0.000001f;
            vb.y -= 0.000001f;
            va.z += 0.000001f;
            vb.w -= 0.000001f;
            result += wne::dot(va, vb);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "wne dot: " << diff.count() << " s\n";
    }
}

void benchCross()
{
    wne::Vector3 va(1.0f, 2.0f, 1.0f);
    wne::Vector3 vb(2.0f, 1.0f, -1.0f);

    std::cout << "Cross bench" << std::endl;
    {
        wne::Vector3 result;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i)
        {
            va.x += 0.000001f;
            vb.y -= 0.000001f;
            result += wne::cross(va, vb);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "wne cross: " << diff.count() << " s\n";
    }
}