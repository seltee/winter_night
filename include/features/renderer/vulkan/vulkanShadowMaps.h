#pragma once
#include "core/core.h"
#include "core/math.h"
#include <vector>

namespace wne
{
    class VulkanDepthBuffer;
    class VulkanShadowMaps
    {
    public:
        void clear();
        uint32 registerShadowMap(VulkanDepthBuffer *shadowMap);
        uint32 getShadowMapsAmount();
        VulkanDepthBuffer *getDepthBuffer(uint32 index);

    protected:
        std::vector<VulkanDepthBuffer *> shadowMaps;
    };
};
