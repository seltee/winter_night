#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
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
        VulkanDepthBuffer *depthBuffer = depthBuffers[i].get();

        vulkanUtils->getCurrentCommandBuffer()->beginDepthPass(depthPass, frameBuffer->getFrameBuffer(), resolition, resolition);
        scene->renderDepth(renderer);
        vulkanUtils->getCurrentCommandBuffer()->endPass();

        /*
        VkMemoryBarrier memBarrier = {};
        memBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        memBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT;

        vkCmdPipelineBarrier(vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer(),
                             VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // conservative but safe
                             VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                             0,
                             1, &memBarrier,
                             0, nullptr,
                             0, nullptr);
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

    for (uint i = 0; i < amountOfCascades; i++)
    {
        auto depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils);
        if (!depthBuffer->setup(resolition, resolition, true))
        {
            std::cout << "unable to create depth buffer" << std::endl;
            return false;
        }
        depthBuffers.emplace_back(std::move(depthBuffer));
    }
    frameBuffers.resize(amountOfCascades);

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
    auto depthBuffer = depthBuffers.at(numOfCascade).get();

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
    depthBuffers.clear();
    frameBuffers.clear();
}

VulkanFrameBuffer *VulkanLightDirectional::getFrameBuffer(int cascade)
{
    VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
    VulkanDepthBuffer *depthBuffer = depthBuffers[cascade].get();
    if (!frameBuffers[cascade])
    {
        frameBuffers[cascade] = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
        if (!frameBuffers[cascade]->setup(depthPass, depthBuffer))
            return nullptr;
    }
    else
    {
        // check if depth pass is still correct
        if (frameBuffers[cascade]->getDepthPass() != depthPass)
        {
            // utils have new one, recreate
            frameBuffers[cascade] = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
            if (!frameBuffers[cascade]->setup(depthPass, depthBuffer))
                return nullptr;
        }
    }
    return frameBuffers[cascade].get();
}