#pragma once
#include "features/renderer/meshArmature.h"
#include "features/renderer/vulkan/vulkanUtils.h"

#define MAX_WEIGHTS_PER_VERTEX 5

namespace wne
{
    struct BoneVertexData
    {
        float weights[MAX_WEIGHTS_PER_VERTEX];
        uint weightsAmount;
    };

    class VulkanMeshArmature : public MeshArmature
    {
    public:
        VulkanMeshArmature(VulkanUtils *vulkanUtils);

        bool setupMatrixBuffer(int boneAmount) override final;
        bool setupBoneWeights(const std::vector<std::shared_ptr<wne::Bone>> &bones) override final;
        void setBoneTransformationMatrix(int index, const Matrix4x4 &data) override final;

        inline int32 getWeightIndexShift() const
        {
            return boneWeightsBufferIndex;
        }
        
        inline int32 getMatrixIndexShift() const
        {
            return bonesBufferIndex;
        }


    protected:
        VulkanUtils *vulkanUtils = nullptr;

        int32 bonesBufferIndex = 0xffffffff;
        int32 boneWeightsBufferIndex = 0xffffffff;
    };
};
