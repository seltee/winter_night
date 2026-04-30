#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanInstanceExtensions.h"
#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanFrame.h"
#include "core/core.h"

#define VULKAN_INSTANCE_REQUIRED_EXTENSIONS 2

namespace WNE
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

    class WNE_API VulkanInstance
    {
    public:
        ~VulkanInstance();

        bool initNT(void *hWnd, uint32 width, uint32 height);
        bool init(uint32 width, uint32 height, VkSurfaceKHR surface);
        void changeSize(uint32 width, uint32 height);
        void render();

    protected:
        uint32_t currentFrame = 0;

        VulkanInstanceExtensions *vulkanInstanceExtensions = nullptr;

        VkInstance instance = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        VulkanShader *defaultShader = nullptr;
        VulkanPipeline *pipeline = nullptr;
        VulkanRenderPass *renderPass = nullptr;
        VulkanFrameBuffer *frameBuffer = nullptr;
        VulkanSwapChain *swapChain = nullptr;
        VulkanDevice *vDevice = nullptr;
        std::vector<VulkanFrame *> frames;

        const char *const instanceExtNames[VULKAN_INSTANCE_REQUIRED_EXTENSIONS] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"};

        bool initInstance();
    };
}