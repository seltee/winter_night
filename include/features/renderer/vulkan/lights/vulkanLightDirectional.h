#pragma once
#include "features/scene/light.h"

namespace wne
{
    class VulkanUtils;
    class VulkanLightDirectional : public Light
    {
    public:
        VulkanLightDirectional(VulkanUtils *vulkanUtils);
        ~VulkanLightDirectional();

        void prepareForRender() override final;

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
