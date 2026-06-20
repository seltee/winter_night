#include "features/renderer/vulkan/vulkanLayers.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

using namespace wne;

VulkanLayers::VulkanLayers()
{
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    for (uint32_t i = 0; i < layerCount; i++)
        layerNames.push_back(strdup( layers[i].layerName));
}

VulkanLayers::~VulkanLayers()
{
    for (auto &layer : layerNames)
        free(layer);
}

int VulkanLayers::getLayersCount()
{
    return layerCount;
}

char **VulkanLayers::getLayersData()
{
    return layerNames.data();
}

bool VulkanLayers::hasLayer(const char *name)
{
    for (auto &layer : layerNames)
    {
        if (strcmp(name, layer) == 0)
            return true;
    }
    Logger::log << "Layer " << name << " not found" << endl;
    return false;
}

void VulkanLayers::dumpLayers()
{
    Logger::log << "List of layers" << endl;
    for (auto &layer : layerNames)
    {
        Logger::log << layer << endl;
    }
}
