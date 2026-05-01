#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanCommandPool.h"
#include "features/renderer/vulkan/vulkanInstanceExtensions.h"
#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanFrame.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "core/core.h"
#include <memory>

#define VULKAN_INSTANCE_REQUIRED_EXTENSIONS 2

namespace wne
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

    class WNE_API VulkanInstance
    {
    public:
        static std::unique_ptr<VulkanInstance> create(void *hwnd, uint32 w, uint32 h)
        {
            auto instance = std::unique_ptr<VulkanInstance>(new VulkanInstance());
            if (!instance->initNT(hwnd, w, h))
                return nullptr;
            return instance;
        }

        ~VulkanInstance();

        void changeSize(uint32 width, uint32 height);

        void setSyncState(bool syncEnabled);
        bool getSyncState();

        void startRendering();
        void finishRendering();

        VulkanUtils *getVulkanUtils()
        {
            return utils.get();
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

        std::unique_ptr<VulkanShader> defaultShader = nullptr;
        std::unique_ptr<VulkanPipeline> pipeline = nullptr;
        std::unique_ptr<VulkanRenderPass> renderPass = nullptr;
        std::unique_ptr<VulkanFrameBuffer> frameBuffer = nullptr;
        std::unique_ptr<VulkanSwapChain> swapChain = nullptr;
        std::unique_ptr<VulkanCommandPool> commandPool = nullptr;
        std::unique_ptr<VulkanDevice> vulkanDevice = nullptr;
        std::unique_ptr<VulkanUtils> utils = nullptr;
        std::vector<std::unique_ptr<VulkanFrame>> frames;

        const char *const instanceExtNames[VULKAN_INSTANCE_REQUIRED_EXTENSIONS] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"};

        VulkanInstance() = default;
        bool initNT(void *hWnd, uint32 width, uint32 height);
        bool init(uint32 width, uint32 height, VkSurfaceKHR surface);
        bool initInstance();
    };
}