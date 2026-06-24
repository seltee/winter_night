#pragma once
#include <memory>
#include "core/api.h"
#include "core/core.h"
#include "features/scene/scene.h"

namespace wne
{
    class Renderer;
    class Window;
    std::shared_ptr<Scene> createWaylandUIScene(Renderer *renderer, Window *window);
}