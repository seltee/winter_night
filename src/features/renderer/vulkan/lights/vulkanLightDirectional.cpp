#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/renderer/vulkan/vulkanUtils.h"

using namespace wne;

VulkanLightDirectional::VulkanLightDirectional(VulkanUtils *vulkanUtils) : Light(Light::Type::Directional)
{
    this->vulkanUtils = vulkanUtils;
    lightId = vulkanUtils->getObjectBuffers()->getNewLightId();
}

VulkanLightDirectional::~VulkanLightDirectional()
{
    vulkanUtils->getObjectBuffers()->freeLightId(lightId);
}

void VulkanLightDirectional::prepareForRender()
{
    vulkanUtils->getObjectBuffers()->updateLightData(lightId, Light::Type::Directional, direction, color);
}
