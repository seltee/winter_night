#include "features/renderer/vulkan/vulkanInstanceExtensions.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <string.h>

VulkanInstanceExtensions::VulkanInstanceExtensions()
{
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extProps(extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extProps.data());
    for (uint32_t i = 0; i < extensionCount; i++)
    {
        extNames.push_back(strdup(extProps[i].extensionName));
    }
}

VulkanInstanceExtensions::~VulkanInstanceExtensions()
{
    for (auto &ext : extNames)
    {
        free(ext);
    }
}

int VulkanInstanceExtensions::getExtensionsCount()
{
    return extensionCount;
}

char **VulkanInstanceExtensions::getExtensionsData()
{
    return extNames.data();
}

bool VulkanInstanceExtensions::hasExtension(const char *name)
{
    for (auto &ext : extNames)
    {
        if (strcmp(name, ext) == 0)
            return true;
    }
    return false;
}