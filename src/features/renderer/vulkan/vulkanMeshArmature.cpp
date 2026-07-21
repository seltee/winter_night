#include "features/renderer/vulkan/vulkanMeshArmature.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <memory>

using namespace wne;

std::mutex VulkanMeshArmature::cacheWeightsMutex;
std::vector<VulkanMeshArmature::CachedArmatureWeights> VulkanMeshArmature::cacheWeightsList;

VulkanMeshArmature::VulkanMeshArmature(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanMeshArmature::~VulkanMeshArmature()
{
    if (bonesBufferIndex != 0xffffffff)
    {
        auto objectBuffers = vulkanUtils->getObjectBuffers();
        objectBuffers->deallocateBonesOfOjbect(bonesBufferIndex);
    }
}

bool VulkanMeshArmature::setupMatrixBuffer(int bonesAmount)
{
    auto objectBuffers = vulkanUtils->getObjectBuffers();
    bonesBufferIndex = objectBuffers->allocateBonesForObject(bonesAmount);
    return true;
}

bool VulkanMeshArmature::setupBoneWeights(const std::shared_ptr<Armature> &armature)
{
    auto bones = armature->getBones();
    auto objectBuffers = vulkanUtils->getObjectBuffers();
    boneWeightsBufferIndex = getCachedWeightsBufferIndex(armature, maxIndex);
    auto boneWeightsBuffer = objectBuffers->getBoneWeightsForObject(boneWeightsBufferIndex);

    boneMatricies.resize(bones.size());

    for (int i = 0; i < maxIndex; i++)
        for (int v = 0; v < MAX_WEIGHTS_PER_VERTEX; v++)
            boneWeightsBuffer[i].boneWeight[v] = 0;

    uint boneIndex = 0;
    for (auto &bone : bones)
    {
        boneMatricies[boneIndex] = bone->getTransform();
        auto indexes = bone->getIndexes();
        auto weights = bone->getWeights();
        uint indexAmount = indexes.size();
        for (uint i = 0; i < indexAmount; i++)
        {
            uint vIndex = indexes[i];
            float weight = weights[i];
            for (int v = 0; v < MAX_WEIGHTS_PER_VERTEX; v++)
            {
                if (boneWeightsBuffer[vIndex].boneWeight[v] == 0.0f)
                {
                    boneWeightsBuffer[vIndex].boneIndex[v] = boneIndex;
                    boneWeightsBuffer[vIndex].boneWeight[v] = weight;
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
        objectBuffers->setBoneMatrix(bonesBufferIndex + index, data * boneMatricies[index]);
    }
}

int32 VulkanMeshArmature::getCachedWeightsBufferIndex(const std::shared_ptr<Armature> &armature, int maxIndex)
{
    std::lock_guard<std::mutex> lock(cacheWeightsMutex);

    for (auto &cachedIndexData : cacheWeightsList)
    {
        if (cachedIndexData.armature.get() == armature.get())
            return cachedIndexData.weightsBufferIndex;
    }

    auto objectBuffers = vulkanUtils->getObjectBuffers();
    int newBoneWeightsBufferIndex = objectBuffers->allocateBoneWeightsForObject(maxIndex);
    cacheWeightsList.emplace_back(CachedArmatureWeights({armature, newBoneWeightsBufferIndex}));
    return newBoneWeightsBufferIndex;
}
