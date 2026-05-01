#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#include "vulkan/vulkan.h"
#include <cstring>

using namespace wne;

VulkanDeviceExtensions::VulkanDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // printf("Device extensions\n");
    for (const auto &extension : availableExtensions)
    {
        // printf("%s\n", extension.extensionName);
        exts.push_back(extension.extensionName);
    }
}

bool VulkanDeviceExtensions::hasExtension(const char *extName)
{
    for (const auto &ext : exts)
    {
        if (strcmp(ext, extName) == 0)
        {
            return true;
        }
    }
    return false;
}