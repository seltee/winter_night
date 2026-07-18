#include "features/renderer/meshArmature.h"

using namespace wne;

bool MeshArmature::setup(std::shared_ptr<Armature> armature)
{
    maxIndex = armature->getMaxIndex();
    boneAmount = armature->getBones().size();
    this->armature = armature;

    setupMatrixBuffer(boneAmount);
    setupBoneWeights(armature->getBones());
}

bool MeshArmature::setupMatrixBuffer(int boneAmount)
{
}

bool MeshArmature::setupBoneWeights(const std::vector<std::shared_ptr<wne::Bone>> &bones)
{
}

void MeshArmature::setBoneTransformationMatrix(int index, const Matrix4x4 &data)
{
}