#include "features/renderer/vulkan/vulkanMaterial.h"
#include "features/renderer/vulkan/vulkanPipeline.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

typedef struct wne::VulkanMaterialDescription
{
    VkDescriptorBufferInfo bufferInfo{};
    VkDescriptorImageInfo imageInfo{};
    VkWriteDescriptorSet descriptorSampler;
} VulkanMaterialDescription;

VulkanMaterial::VulkanMaterial(VulkanUtils *vulkanUtils)
{
    // auto device = vulkanUtils->getVulkanDevice()->getDevice();
    this->vulkanUtils = vulkanUtils;
    this->vkDescriptorSet = vkDescriptorSet;

    // create uniform buffer to pass parameters of the shader
    // VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    // vulkanUtils->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBuffersMemory);
    // vkMapMemory(device, uniformBuffersMemory, 0, bufferSize, 0, &uniformBuffersMapped);

    // descriptorSet = std::make_unique<VulkanDescriptorSet>(vulkanUtils->getVulkanDevice());
    // description = new VulkanMaterialDescription();
}

VulkanMaterial::~VulkanMaterial()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (uniformBuffer)
        vkDestroyBuffer(device, uniformBuffer, nullptr);
    if (uniformBuffersMemory)
        vkFreeMemory(device, uniformBuffersMemory, nullptr);
    delete description;
}

void setup(int maxFrames)
{
}

std::shared_ptr<Material> VulkanMaterial::createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture)
{
    std::shared_ptr<VulkanMaterial> material = std::make_shared<VulkanMaterial>(vulkanUtils);
    material->setAlbedo(texture);
    material->rebuild();
    return material;
}

void VulkanMaterial::rebuild()
{
    // auto device = vulkanUtils->getVulkanDevice()->getDevice();
    isDirty = false;

    /*
    description->bufferInfo.buffer = uniformBuffer;
    description->bufferInfo.offset = 0;
    description->bufferInfo.range = sizeof(UniformBufferObject);

    description->imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    description->imageInfo.imageView = nullptr;
    description->imageInfo.sampler = vulkanSampler->getTextureSampler();

    description->descriptorSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    description->descriptorSampler.dstSet = vkDescriptorSet;
    description->descriptorSampler.dstBinding = 1;
    description->descriptorSampler.dstArrayElement = 0;
    description->descriptorSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    description->descriptorSampler.descriptorCount = 1;
    description->descriptorSampler.pImageInfo = &description->imageInfo;

    vkUpdateDescriptorSets(device, 1, &description->descriptorSampler, 1, nullptr);
    */
}

void VulkanMaterial::bind(const Matrix4x4 &mMVP)
{
    if (isDirty)
        rebuild();

    PushConstantObject pco;
    pco.mvp = mMVP;

    vkCmdPushConstants(
        vulkanUtils->getCurrentCommandBuffer(),
        vulkanUtils->getVulkanPipeline()->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT, // Stage flags
        0,                          // Offset
        sizeof(PushConstantObject), // Size
        &pco                        // Data pointer
    );
}