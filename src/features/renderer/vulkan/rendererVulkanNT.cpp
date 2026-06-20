#include "features/renderer/vulkan/rendererVulkanNT.h"
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

std::unique_ptr<RendererVulkanNT> RendererVulkanNT::create(void *hwnd, int32 width, int32 height)
{
    auto instance = std::unique_ptr<RendererVulkanNT>(new RendererVulkanNT());
    if (!instance->setup(hwnd, width, height))
        return nullptr;
    return instance;
}

bool RendererVulkanNT::setup(void *hWnd, int32 width, int32 height)
{
    state = new VulkanRendererState();
    instance = VulkanInstance::createNT(hWnd, width, height);
    if (!instance)
        return false;

    setupDefaults();
    return true;
}