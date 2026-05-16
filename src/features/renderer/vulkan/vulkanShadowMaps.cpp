#include "features/renderer/vulkan/vulkanShadowMaps.h"

using namespace wne;

void VulkanShadowMaps::clear()
{
    shadowMaps.clear();
}

uint32 VulkanShadowMaps::registerShadowMap(VulkanDepthBuffer *shadowMap)
{
    shadowMaps.push_back(shadowMap);
    return (uint32)shadowMaps.size() - 1;
}

uint32 VulkanShadowMaps::getShadowMapsAmount()
{
    return (uint32)shadowMaps.size();
}

VulkanDepthBuffer *VulkanShadowMaps::getDepthBuffer(uint32 index)
{
    return shadowMaps[index];
}