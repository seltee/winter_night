#pragma once
#include <vector>
#include <chrono>
#include "features/os/window.h"
#include "features/data/model.h"
#include "features/scene/scene.h"
#include "features/scene/object.h"
#include "features/scene/objectMesh.h"
#include "core/api.h"

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
    };
}