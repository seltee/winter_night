#include "features/renderer/vulkan/rendererVulkanWayland.h"
#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/lights/vulkanLight.h"
#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/data/image.h"
#include "utils/primitives.h"
#include "core/core.h"
#include "core/math.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

using namespace wne;

std::unique_ptr<RendererVulkanWayland> RendererVulkanWayland::create(void *wlDisplay, void *wlSurface)
{
    auto instance = std::unique_ptr<RendererVulkanWayland>(new RendererVulkanWayland());
    if (!instance->setup(wlDisplay, wlSurface))
        return nullptr;
    return instance;
}

bool RendererVulkanWayland::setup(void *wlDisplay, void *wlSurface)
{
    state = new VulkanRendererState();
    instance = VulkanInstance::createLinuxWayland(wlDisplay, wlSurface);
    if (!instance)
        return false;

    setupDefaults();
    return true;
}