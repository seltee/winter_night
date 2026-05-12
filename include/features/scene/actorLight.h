#pragma once
#include "features/scene/light.h"
#include "features/scene/actor.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class WNE_API ActorLight : public Actor
    {
    public:
        ActorLight(std::shared_ptr<Light> light);
        static std::shared_ptr<ActorLight> create(std::shared_ptr<Light> light);

        void eventSetScene(Scene *oldScene, Scene *newScene) override final;

        inline std::shared_ptr<Light> getLight()
        {
            return light;
        }

    protected:
        std::shared_ptr<Light> light;
    };
};
