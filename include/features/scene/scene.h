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

        inline void setAmbientLight(float r, float g, float b, float a = 1.0f)
        {
            ambientLightColor = Vector4(r, g, b, a);
        }

        inline void setAmbientLight(Vector4 color)
        {
            ambientLightColor = color;
        }

        inline Vector4 getAmbientLight()
        {
            return ambientLightColor;
        }

    protected:
        std::vector<std::shared_ptr<Actor>> actors;
        std::shared_ptr<ActorCamera> actorCamera;

        Vector4 ambientLightColor = {0.8f, 0.8f, 0.8f, 1.0f};
    };
};
