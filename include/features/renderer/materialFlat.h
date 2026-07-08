#pragma once
#include "features/renderer/material.h"

namespace wne
{
    class MaterialFlat : public Material
    {
    public:
        virtual void setAlbedoTexture(std::shared_ptr<Texture> albedoTexture);
        virtual std::shared_ptr<Texture> getAlbedoTexture();
    };
};
