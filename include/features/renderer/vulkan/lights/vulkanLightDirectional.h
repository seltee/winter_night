#pragma once
#include "features/data/lightDirectional.h"
#include <vector>
#include <memory>

namespace wne
{
    class VulkanUtils;
    class VulkanDepthBuffer;
    class VulkanDepthPass;
    class VulkanFrameBuffer;

    class WNE_API VulkanLightDirectional : public LightDirectional
    {
    public:
        VulkanLightDirectional(VulkanUtils *vulkanUtils);
        ~VulkanLightDirectional();

        void renderShadows(Scene *scene, ActorCamera *camera) override final;
        bool enableShadows(uint amountOfCascades, uint resolition) override final;
        void disableShadows() override final;
        void prepareForRender() override final;

    protected:
        void destroyShadows();

        uint amountOfCascades = 0;
        uint resolition = 0;

        VulkanUtils *vulkanUtils = nullptr;
        std::vector<std::unique_ptr<VulkanDepthBuffer>> depthBuffers;
        std::vector<std::unique_ptr<VulkanFrameBuffer>> frameBuffers;
    };
};
