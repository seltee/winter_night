#pragma once
#include <vector>
#include <chrono>
#include "core/api.h"
#include "core/math.h"
#include "core/jobQueue.h"
#include "features/os/window.h"
#include "features/data/model.h"
#include "features/data/font.h"
#include "features/scene/scene.h"
#include "features/scene/object.h"
#include "features/scene/actor.h"
#include "features/scene/actorCamera.h"
#include "features/scene/actorCameraFlying.h"
#include "features/scene/actorMesh.h"
#include "features/scene/actorAnimatedMesh.h"
#include "features/scene/actorTerrain.h"
#include "features/scene/actorSprite.h"
#include "features/scene/actorLight.h"
#include "features/scene/actorUI.h"
#include "features/data/light.h"
#include "features/data/lightDirectional.h"
#include "features/data/image.h"
#include "features/data/imageAtmo.h"
#include "features/camera/cameraOrtho.h"
#include "features/camera/cameraPerspective.h"
#include "features/loaders/loader3d.h"
#include "features/logger/logger.h"
#include "features/renderer/text.h"
#include "features/static/media.h"
#include "utils/primitives.h"
#include <memory>

namespace wne
{
    class WNE_API Engine
    {
    protected:
        Engine();
        static Engine *instance;
        static std::shared_ptr<Font> font;
        std::vector<Window *> windows;
        std::chrono::_V2::system_clock::time_point timePoint;
        float fpsCounter = 0.0f;
        int framesCounter = 0;
        JobQueue jobQueue;

    public:
        static Engine *getInstance();
        void registerWindow(Window *window);
        void unregisterWindow(Window *window);

        void enableLogger();

        float update();

        Media media;

        inline JobQueue &getJobQueue()
        {
            return jobQueue;
        }
    };
}