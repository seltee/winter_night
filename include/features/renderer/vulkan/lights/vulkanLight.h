#pragma once
#include "features/data/light.h"

namespace wne
{
    class VulkanUtils;
    class VulkanLight : public Light
    {
    public:
        VulkanLight(VulkanUtils *vulkanUtils, Light::Type type);
        ~VulkanLight();

        void prepareForRender() override final;

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
