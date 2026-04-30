#pragma once
#include <vector>
#include "features/os/window.h"
#include "core/api.h"
#include <chrono>

namespace WNE
{
    class WNE_API Engine
    {
    protected:
        Engine();
        static Engine *instance;
        std::vector<Window *> windows;
        std::chrono::_V2::system_clock::time_point timePoint;

    public:
        static Engine *getInstance();

        void registerWindow(Window *window);
        void unregisterWindow(Window *window);

        float update();
        void render();
    };
}