#include "engine.h"
#include <stdio.h>
#include <iostream>

// Iterations
constexpr int iterations = 1'000'000'000; // 1kkk
constexpr int iterationsLow = 10'000'000; // 10kk

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
void benchMult3x3(wne::Matrix3x3 ma, wne::Matrix3x3 mb);
void benchMult4x4(wne::Matrix4x4 ma, wne::Matrix4x4 mb);
void benchDet3x3(wne::Matrix3x3 m);
void benchDet4x4(wne::Matrix4x4 m);
void benchInverse3x3(wne::Matrix3x3 m);
void benchInverse4x4(wne::Matrix4x4 m);

int main()
{
    std::cout << "Math tests" << std::endl;

    // Dot benches
    benchDotVec2();
    benchDotVec3();
    benchDotVec4();
    benchCross();
    benchMult3x3(
        wne::Matrix3x3(
            1, 0, 2,
            4, 3, 1,
            8, 5, 4),
        wne::Matrix3x3(
            1, 2, 0,
            1, 3, 1,
            2, 1, 2));
    benchMult4x4(
        wne::Matrix4x4(
            1, 0, 2, 1,
            4, 3, 1, 7,
            8, 5, 4, 2,
            1, 3, 0, 2),
        wne::Matrix4x4(
            1, 2, 0, 5,
            1, 3, 1, 4,
            2, 1, 2, 3,
            3, 2, 1, 0));
    benchDet3x3(
        wne::Matrix3x3(
            1, 0, 2,
            4, 3, 1,
            8, 5, 4));
    benchDet4x4(
        wne::Matrix4x4(
            1, 2, 0, 5,
            1, 3, 1, 4,
            2, 1, 2, 3,
            3, 2, 1, 0));
    benchInverse3x3(wne::Matrix3x3(
        1, 0, 2,
        4, 3, 1,
        8, 5, 4));
    benchInverse4x4(wne::Matrix4x4(
        1, 2, 0, 5,
        1, 3, 1, 4,
        2, 1, 2, 3,
        3, 2, 1, 0));

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
        std::cout << "Result: " << diff.count() << " s\n";
    }
}

void benchMult3x3(wne::Matrix3x3 ma, wne::Matrix3x3 mb)
{
    std::cout << "Mul Matrix 3x3 bench" << std::endl;
    {
        wne::Matrix3x3 result;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterationsLow; ++i)
        {
            result = ma * mb;
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "Result: " << diff.count() << " s\n";
    }
}

void benchMult4x4(wne::Matrix4x4 ma, wne::Matrix4x4 mb)
{
    std::cout << "Mul Matrix 4x4 bench" << std::endl;
    {
        wne::Matrix4x4 result;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterationsLow; ++i)
        {
            result = ma * mb;
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "Result: " << diff.count() << " s\n";
    }
}

void benchDet3x3(wne::Matrix3x3 m)
{
    std::cout << "Det Matrix 3x3 bench" << std::endl;
    {
        float result;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterationsLow; ++i)
        {
            result += wne::determinant(m);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "Result: " << diff.count() << " s\n";
    }
}

void benchDet4x4(wne::Matrix4x4 m)
{
    std::cout << "Det Matrix 4x4 bench" << std::endl;
    {
        float result;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterationsLow; ++i)
        {
            result += wne::determinant(m);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "Result: " << diff.count() << " s\n";
    }
}

void benchInverse3x3(wne::Matrix3x3 m)
{
    std::cout << "Inverse Matrix 3x3 bench" << std::endl;
    {
        auto start = std::chrono::high_resolution_clock::now();
        wne::Matrix3x3 result = wne::Matrix3x3::identity();
        for (int i = 0; i < iterationsLow; ++i)
        {
            result = result * wne::inverse(m);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "Result: " << diff.count() << " s\n";
    }
}

void benchInverse4x4(wne::Matrix4x4 m)
{
    std::cout << "Inverse Matrix 4x4 bench" << std::endl;
    {
        wne::Matrix4x4 result = wne::Matrix4x4::identity();
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterationsLow; ++i)
        {
            result = result * wne::inverse(m);
        }
        auto end = std::chrono::high_resolution_clock::now();
        doNotOptimize(result);
        std::chrono::duration<double> diff = end - start;
        std::cout << "Result: " << diff.count() << " s\n";
    }
}
