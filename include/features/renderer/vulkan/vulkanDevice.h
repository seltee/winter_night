#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"

#define VULKAN_DEVICE_REQUIRED_EXTENSIONS 1

namespace wne
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
        ~VulkanDevice();

        bool setup();

        bool createBuffer(uint64 size, uint32 usage, uint32 properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        bool copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64 size);

        inline VkDevice getDevice()
        {
            return device;
        }

        inline VkPhysicalDevice getPhysicalDevice()
        {
            return physicalDevice;
        }

        inline VkSurfaceKHR getSurface()
        {
            return surface;
        }

    protected:
        VkInstance instance;
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;

        const char *const deviceExtNames[VULKAN_DEVICE_REQUIRED_EXTENSIONS] = {
            "VK_KHR_swapchain"};
    };
}