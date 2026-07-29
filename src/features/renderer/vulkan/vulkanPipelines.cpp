
#include "features/renderer/vulkan/vulkanPipelines.h"
#include "features/renderer/vulkan/vulkanShadowMaps.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineUniversal.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>
#include <cmath>

using namespace wne;

VulkanPipelines::VulkanPipelines()
{
}
