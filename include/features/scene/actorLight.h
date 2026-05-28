#pragma once
#include "features/data/light.h"
#include "features/scene/actor.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class WNE_API ActorLight : public Actor
    {
    public:
        ActorLight(Renderer *renderer, std::shared_ptr<Light> light);

        void eventSetScene(Scene *oldScene, Scene *newScene) override final;
        
        void update(float delta);

        void updateRenderFlag(ActorCamera *camera) override;
        
        inline std::shared_ptr<Light> getLight()
        {
            return light;
        }

    protected:
        std::shared_ptr<Light> light;
    };
};
