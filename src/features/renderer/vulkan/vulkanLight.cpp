#include "features/renderer/vulkan/vulkanLight.h"
#include "features/renderer/vulkan/vulkanUtils.h"

using namespace wne;

VulkanLight::VulkanLight(VulkanUtils *vulkanUtils, Light::Type type) : Light(type)
{
    this->vulkanUtils = vulkanUtils;
    lightId = vulkanUtils->getObjectBuffers()->getNewLightId();
}

VulkanLight::~VulkanLight()
{
    vulkanUtils->getObjectBuffers()->freeLightId(lightId);
}

void VulkanLight::prepareForRender()
{
    vulkanUtils->getObjectBuffers()->updateLightData(lightId, type, getAffectRadius(), position, getRealDirection(), color);
}
