#pragma once
#include <memory>
#include <vector>
#include "features/scene/object.h"
#include "features/scene/actor.h"
#include "features/scene/actorCamera.h"
#include "core/api.h"
#include "core/data.h"
#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class Renderer;

    class WNE_API Scene
    {
    public:
        static std::shared_ptr<Scene> create();

        void update(float delta);
        void render(Renderer *renderer);

        void addActor(std::shared_ptr<Actor> actor);
        void setCamera(std::shared_ptr<ActorCamera> actorCamera);

    protected:
        std::vector<std::shared_ptr<Actor>> actors;
        std::shared_ptr<ActorCamera> actorCamera;
    };
};
