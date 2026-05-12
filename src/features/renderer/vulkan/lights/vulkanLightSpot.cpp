#include "features/renderer/vulkan/lights/vulkanLightSpot.h"
#include "features/renderer/vulkan/vulkanUtils.h"

using namespace wne;

VulkanLightSpot::VulkanLightSpot(VulkanUtils *vulkanUtils) : Light(Light::Type::Spot)
{
    this->vulkanUtils = vulkanUtils;
    lightId = vulkanUtils->getObjectBuffers()->getNewLightId();
}

VulkanLightSpot::~VulkanLightSpot()
{
    vulkanUtils->getObjectBuffers()->freeLightId(lightId);
}
