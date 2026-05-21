#include "features/renderer/vulkan/lights/vulkanLight.h"
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

void VulkanLight::prepareForRender(const Vector3 &cameraPosition)
{
    vulkanUtils->getObjectBuffers()->updateLightData(
        lightId,
        type,
        affectRadius,
        cutOff,
        outerCutOff,
        0, 0, 0,
        position,
        realDirection,
        color);
}
