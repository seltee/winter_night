#pragma once
#include "features/data/light.h"
#include "core/core.h"
#include "core/api.h"
#include "features/renderer/texture.h"
#include <memory>
namespace wne
{
    class WNE_API LightDirectional : public Light
    {
    public:
        LightDirectional();

        virtual bool enableShadows(uint amountOfCascades, uint resolition);
        virtual void disableShadows();

        virtual std::shared_ptr<Texture> getCascadeAsTexture(int numOfCascade);
    };
};
