#pragma once
#include <vector>
#include <chrono>
#include "core/api.h"
#include "core/math.h"
#include "features/os/window.h"
#include "features/data/model.h"
#include "features/scene/scene.h"
#include "features/scene/object.h"
#include "features/scene/actor.h"
#include "features/scene/actorCamera.h"
#include "features/scene/actorCameraFlying.h"
#include "features/scene/actorMesh.h"
#include "features/scene/actorTerrain.h"
#include "features/scene/actorLight.h"
#include "features/data/light.h"
#include "features/data/lightDirectional.h"
#include "features/data/image.h"
#include "features/data/imageAthmo.h"
#include "features/camera/cameraOrtho.h"
#include "features/camera/cameraPerspective.h"
#include "features/loaders/loader3d.h"
#include "utils/primitives.h"

namespace wne
{
    class WNE_API Engine
    {
    protected:
        Engine();
        static Engine *instance;
        std::vector<Window *> windows;
        std::chrono::_V2::system_clock::time_point timePoint;
        float fpsCounter = 0.0f;
        int framesCounter = 0;

    public:
        static Engine *getInstance();

        void registerWindow(Window *window);
        void unregisterWindow(Window *window);

        float update();
    };
}