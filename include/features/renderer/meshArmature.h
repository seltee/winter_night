#pragma once
#include "features/data/armature.h"
#include "core/core.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class MeshArmature
    {
    public:
        bool setup(std::shared_ptr<Armature> armature);

        virtual bool setupMatrixBuffer(int boneAmount);
        virtual bool setupBoneWeights(const std::vector<std::shared_ptr<wne::Bone>> &bones);
        virtual void setBoneTransformationMatrix(int index, const Matrix4x4 &data);

    protected:
        int maxIndex = 0;
        int boneAmount = 0;
        std::shared_ptr<Armature> armature;
        std::vector<Matrix4x4> boneMatricies;
    };
}