#include "features/renderer/vulkan/lights/vulkanLightOmni.h"
#include "features/renderer/vulkan/vulkanUtils.h"

using namespace wne;

VulkanLightOmni::VulkanLightOmni(VulkanUtils *vulkanUtils) : Light(Light::Type::Omni)
{
    this->vulkanUtils = vulkanUtils;
    lightId = vulkanUtils->getObjectBuffers()->getNewLightId();
}

VulkanLightOmni::~VulkanLightOmni()
{
    vulkanUtils->getObjectBuffers()->freeLightId(lightId);
}
