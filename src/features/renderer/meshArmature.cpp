#include "features/renderer/meshArmature.h"

using namespace wne;

bool MeshArmature::setup(std::shared_ptr<Armature> armature)
{
    maxIndex = armature->getMaxIndex();
    boneAmount = armature->getBones().size();
    this->armature = armature;

    setupMatrixBuffer(boneAmount);
    setupBoneWeights(armature);
    return true;
}

bool MeshArmature::setupMatrixBuffer(int boneAmount)
{
    return false;
}

bool MeshArmature::setupBoneWeights(const std::shared_ptr<Armature> &armature)
{
    return false;
}

void MeshArmature::setBoneTransformationMatrix(int index, const Matrix4x4 &data)
{
}