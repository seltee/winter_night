#include "features/renderer/vulkan/vulkanCommandPool.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

using namespace WNE;

VulkanCommandPool::VulkanCommandPool(VkDevice device, VkPhysicalDevice physicalDevice)
{
    this->device = device;
    this->physicalDevice = physicalDevice;
}

VulkanCommandPool::~VulkanCommandPool()
{
}

bool VulkanCommandPool::setup(VkSurfaceKHR surface)
{
    // command pool
    VulkanQueueFamilies vulkanQueueFamilies;
    vulkanQueueFamilies.setup(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = vulkanQueueFamilies.getGraphicsFamily().value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        std::cout << "Failed to create command pool" << std::endl;
        return false;
    }

    return true;
}