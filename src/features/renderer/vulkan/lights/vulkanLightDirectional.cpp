#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/renderer.h"
#include "features/scene/scene.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanLightDirectional::VulkanLightDirectional(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
    lightId = vulkanUtils->getObjectBuffers()->getNewLightId();
}

VulkanLightDirectional::~VulkanLightDirectional()
{
    destroyShadows();
    vulkanUtils->getObjectBuffers()->freeLightId(lightId);
}

void VulkanLightDirectional::renderShadows(Renderer *renderer, Scene *scene, ActorCamera *camera)
{
    for (uint i = 0; i < amountOfCascades; i++)
    {
        VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
        VulkanFrameBuffer *frameBuffer = getFrameBuffer(i);

        Matrix4x4 mProjection = makeOrthographicProjectionMatrix(-60.0f, 60.0f, 60.0f, -60.0f, 0.0f, 80.0f);

        Vector3 projectionPosition = position.xyz() + realDirection.xyz() * 40.0f;
        Matrix4x4 model = Matrix4x4::translation(projectionPosition);
        model = model * Matrix4x4(lookAt(projectionPosition, position.xyz()));
        Matrix4x4 invModelMatrix = inverse(model);
        Matrix4x4 mVP = mProjection * invModelMatrix;

        // current queue building state
        auto state = (VulkanRendererState *)renderer->getState();
        state->setViewProjectionMatrix(mVP);
        state->setVulkanLightCascadeData(cascades[i].get());

        vulkanUtils->getCurrentCommandBuffer()->beginDepthPass(depthPass, frameBuffer->getFrameBuffer(), resolition, resolition);
        scene->renderDepthShadow(renderer);
        vulkanUtils->getCurrentCommandBuffer()->endPass();

                /*
        VkMemoryBarrier memBarrier = {};
        memBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;           // Correct for depth writes
        memBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT; // What comes next needs

        vkCmdPipelineBarrier(
            vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer(),
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,                                   // End of depth writes
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // Before next reads/updates
            0,
            1, &memBarrier,
            0, nullptr,
            0, nullptr);

        VkImageMemoryBarrier imageBarrier = {};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageBarrier.image = depthImage;
        imageBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier(cmd,
                             VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0, nullptr, 0, nullptr,
                             1, &imageBarrier);
        */
        /*
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // or whatever your final layout in renderpass was
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.image = depthBuffer->getDepthImage();
        barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier(vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer(),
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0, nullptr, 0, nullptr,
                             1, &barrier);
        */
    }
}

bool VulkanLightDirectional::enableShadows(uint amountOfCascades, uint resolition)
{
    destroyShadows();
    if (amountOfCascades == 0)
        return true;

    cascades.resize(amountOfCascades);
    for (uint i = 0; i < amountOfCascades; i++)
    {
        cascades[i] = std::make_unique<VulkanLightCascadeData>(vulkanUtils);
        if (!cascades[i]->setup(resolition))
        {
            std::cout << "unable to create depth buffer" << std::endl;
            return false;
        }
    }

    this->amountOfCascades = amountOfCascades;
    this->resolition = resolition;

    return true;
}

void VulkanLightDirectional::disableShadows()
{
    destroyShadows();
}

void VulkanLightDirectional::prepareForRender()
{
    vulkanUtils->getObjectBuffers()->updateLightData(
        lightId,
        type,
        affectRadius,
        cutOff,
        outerCutOff,
        position,
        realDirection,
        color);
}

std::shared_ptr<Texture> VulkanLightDirectional::getCascadeAsTexture(int numOfCascade)
{
    auto depthBuffer = cascades[numOfCascade]->getDepthBuffer();

    std::shared_ptr<VulkanTexture> texture = std::make_shared<VulkanTexture>(vulkanUtils);
    if (!texture->setup(depthBuffer->getDepthImageView()))
    {
        std::cout << "Unable to create texture from image view for shadow cascade" << std::endl;
        return nullptr;
    }
    return texture;
}

void VulkanLightDirectional::destroyShadows()
{
    amountOfCascades = 0;
    cascades.clear();
}

VulkanFrameBuffer *VulkanLightDirectional::getFrameBuffer(int cascade)
{
    VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
    return cascades[cascade]->getFrameBuffer(depthPass);
}