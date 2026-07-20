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
    uint32 maxIndex = armature->getMaxIndex();
    boneWeightsBufferIndex = objectBuffers->allocateBoneWeightsForObject(maxIndex);
    auto boneWeightsBuffer = objectBuffers->getBoneWeightsForObject(boneWeightsBufferIndex);

    boneMatricies.resize(bones.size());

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
    // Logger::log << "BONE MATRIIES " << endl;
    auto objectBuffers = vulkanUtils->getObjectBuffers();
    if (index >= 0 && index < maxIndex)
    {
        // Logger::log << "SET BONE MATRIX " << index << " (" << bonesBufferIndex << ")" << endl;
        // Logger::log << data[0][0] << " " << data[1][0] << " " << data[2][0] << " " << data[3][0] << endl;
        // Logger::log << data[0][1] << " " << data[1][1] << " " << data[2][1] << " " << data[3][1] << endl;
        // Logger::log << data[0][2] << " " << data[1][2] << " " << data[2][2] << " " << data[3][2] << endl;
        // Logger::log << data[0][3] << " " << data[1][3] << " " << data[2][3] << " " << data[3][3] << endl;
        objectBuffers->setBoneMatrix(bonesBufferIndex + index, data * boneMatricies[index]);
    }
}