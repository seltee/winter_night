#include "engine.h"
#include "features/os/window.h"
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace wne;

Engine *Engine::instance = nullptr;

Engine::Engine()
{
    timePoint = std::chrono::high_resolution_clock::now();
}

Engine *Engine::getInstance()
{
    if (!instance)
    {
        instance = new Engine();
    }
    return instance;
}

void Engine::registerWindow(Window *window)
{
    windows.push_back(window);
}

void Engine::unregisterWindow(Window *window)
{
    windows.erase(
        std::remove(windows.begin(), windows.end(), window),
        windows.end());
}

float Engine::update()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto deltaUs = std::chrono::duration_cast<std::chrono::microseconds>(now - timePoint).count();
    timePoint = now;
    float delta = std::min((float)deltaUs / 1000000.0f, 0.2f);

    for (auto &it : windows)
    {
        it->update(delta);
    }

    for (auto &it : windows)
    {
        it->render();
    }

    fpsCounter += delta;
    framesCounter++;
    if (fpsCounter > 1.0f)
    {
        std::cout << "FPS " << framesCounter << std::endl;
        fpsCounter -= 1.0f;
        framesCounter = 0;
    }

    return delta;
}
