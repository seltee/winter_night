#include "features/renderer/vulkan/vulkanInstanceExtensions.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <string.h>
#include <iostream>

using namespace wne;

VulkanInstanceExtensions::VulkanInstanceExtensions()
{
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extProps(extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extProps.data());
    for (uint32_t i = 0; i < extensionCount; i++)
        extNames.push_back(strdup(extProps[i].extensionName));
}

VulkanInstanceExtensions::~VulkanInstanceExtensions()
{
    for (auto &ext : extNames)
        free(ext);
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
    Logger::log << "Extension " << name << " not found" << endl;
    return false;
}

void VulkanInstanceExtensions::dumpExtensions()
{
    Logger::log << "List of extensions" << endl;
    for (auto &ext : extNames)
    {
        Logger::log << ext << endl;
    }
}