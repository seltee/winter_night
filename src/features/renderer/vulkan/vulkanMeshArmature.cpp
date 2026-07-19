#include "features/renderer/vulkan/vulkanMeshArmature.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <memory>

using namespace wne;

VulkanMeshArmature::VulkanMeshArmature(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

bool VulkanMeshArmature::setupMatrixBuffer(int bonesAmount)
{
    auto objectBuffers = vulkanUtils->getObjectBuffers();
    bonesBufferIndex = objectBuffers->allocateBonesForObject(bonesAmount);
    return true;
}

bool VulkanMeshArmature::setupBoneWeights(const std::vector<std::shared_ptr<wne::Bone>> &bones)
{
    auto objectBuffers = vulkanUtils->getObjectBuffers();
    boneWeightsBufferIndex = objectBuffers->allocateBoneWeightsForObject(armature->getMaxIndex());
    auto boneWeightsBuffer = objectBuffers->getBoneWeightsForObject(boneWeightsBufferIndex);

    uint boneIndex = 0;
    for (auto &bone : bones)
    {
        auto indexes = bone->getIndexes();
        auto weights = bone->getWeights();
        uint indexAmount = indexes.size();
        for (uint i = 0; i < indexAmount; i++)
        {
            uint vIndex = indexes[i];
            float weight = weights[i];
            for (int i = 0; i < MAX_WEIGHTS_PER_VERTEX; i++)
            {
                if (boneWeightsBuffer[vIndex].boneWeight[i] == 0.0f)
                {
                    boneWeightsBuffer[vIndex].boneIndex[i] = boneIndex;
                    boneWeightsBuffer[vIndex].boneWeight[i] = weight;
                    break;
                }
            }
        }
        boneIndex++;
    }

    return true;
}

void VulkanMeshArmature::setBoneTransformationMatrix(int index, const Matrix4x4 &data)
{
    auto objectBuffers = vulkanUtils->getObjectBuffers();
    if (index >= 0 && index < maxIndex)
    {
        objectBuffers->setBoneMatrix(bonesBufferIndex + index, data);
    }
}