#include "features/renderer/vulkan/vulkanMeshArmature.h"
#include "vulkan/vulkan.h"
#include <memory>

using namespace wne;

VulkanMeshArmature::VulkanMeshArmature(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

bool VulkanMeshArmature::setupMatrixBuffer(int boneAmount)
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();

    uint bufferSize = (uint)boneAmount * sizeof(Matrix4x4);
    vulkanUtils->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        boneMatricies,
        boneMatriciesMemory);

    vkMapMemory(device, boneMatriciesMemory, 0, bufferSize, 0, (void **)&boneMatriciesMapped);
    for (int i = 0; i < boneAmount; i++)
    {
        boneMatriciesMapped[i] = Matrix4x4::identity();
    }
}

bool VulkanMeshArmature::setupBoneWeights(const std::vector<std::shared_ptr<wne::Bone>> &bones)
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();

    uint bufferSize = maxIndex * sizeof(BoneVertexData);
    vulkanUtils->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexIndexes,
        vertexIndexesMemory);

    vkMapMemory(device, vertexIndexesMemory, 0, bufferSize, 0, (void **)&vertexIndexesMapped);
    memset(vertexIndexesMapped, 0, bufferSize);

    for (auto &bone : bones)
    {
        auto indexes = bone->getIndexes();
        uint indexAmount = indexes.size();
        for (uint i = 0; i < indexAmount; i++)
        {
        }
    }
}

void VulkanMeshArmature::setBoneTransformationMatrix(int index, const Matrix4x4 &data)
{
    if (index >= 0 && index < maxIndex)
    {
        boneMatriciesMapped[index] = data;
    }
}