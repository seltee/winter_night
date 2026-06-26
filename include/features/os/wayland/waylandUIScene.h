#pragma once
#include <memory>
#include "core/api.h"
#include "core/core.h"
#include "features/scene/scene.h"

namespace wne
{
    class Renderer;
    class Window;
    class CameraOrtho;
    class ActorUI;
    class UINodeText;

    struct WaylandUIScene{
        std::shared_ptr<Scene> scene;
        std::shared_ptr<CameraOrtho> camera;
        std::shared_ptr<ActorUI> actor;
        std::shared_ptr<UINodeText> caption;
    };
    WaylandUIScene createWaylandUIScene(Renderer *renderer, Window *window);
}