#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanCommandPool.h"
#include "features/renderer/vulkan/vulkanInstanceExtensions.h"
#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanFrame.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineColored.h"
#include "core/core.h"
#include <memory>

#define VULKAN_INSTANCE_REQUIRED_EXTENSIONS 2

namespace wne
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

    class WNE_API VulkanInstance
    {
    public:
        ~VulkanInstance();
        static std::unique_ptr<VulkanInstance> create(void *hwnd);

        void changeSize();

        void setSyncState(bool syncEnabled);
        bool getSyncState();

        void startRendering();
        void beginDepthPass();
        void finishDepthPass();
        void beginRenderPass();
        void finishRendering();

        VulkanUtils *getVulkanUtils()
        {
            return vulkanUtils.get();
        }

        VulkanDevice *getVulkanDevice()
        {
            return vulkanDevice.get();
        }

        VulkanFrame *getCurrentFrame()
        {
            return frames[currentFrame].get();
        }

    protected:
        uint32 width = 0,
               height = 0;
        uint32_t currentFrame = 0;
        bool isImmidiateSwap = false;

        VulkanInstanceExtensions *vulkanInstanceExtensions = nullptr;

        VkInstance instance = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        std::unique_ptr<VulkanRenderPass> renderPass = nullptr;
        std::unique_ptr<VulkanRenderPass> depthPass = nullptr;
        std::unique_ptr<VulkanFrameBuffer> frameColorBuffer = nullptr;
        std::unique_ptr<VulkanFrameBuffer> frameDepthBuffer = nullptr;
        std::unique_ptr<VulkanDepthBuffer> depthBuffer = nullptr;
        std::unique_ptr<VulkanSwapChain> swapChain = nullptr;
        std::unique_ptr<VulkanCommandPool> commandPool = nullptr;
        std::unique_ptr<VulkanDevice> vulkanDevice = nullptr;
        std::unique_ptr<VulkanUtils> vulkanUtils = nullptr;
        std::vector<std::unique_ptr<VulkanFrame>> frames;

        const char *const instanceExtNames[VULKAN_INSTANCE_REQUIRED_EXTENSIONS] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"};

        VulkanInstance() = default;
        bool initNT(void *hWnd);
        bool init(VkSurfaceKHR surface);
        bool initInstance();
    };
}