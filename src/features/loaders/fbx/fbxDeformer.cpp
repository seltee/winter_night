#include "features/loaders/fbx/fbxDeformer.h"
#include "features/logger/logger.h"

using namespace wne;

FBXDeformer::FBXDeformer(FBXNode &node)
{
    id = node.getLong(0);
    const char *charName = node.getString(1);
    name = std::string(charName);
    const char *charType = node.getString(2);
    typeName = std::string(charType);

    isSkinFlag = (typeName == "Skin");
    isClusterFlag = (typeName == "Cluster");

    auto indexNode = node.findNode("Indexes");
    auto weightNode = node.findNode("Weights");
    auto transformNode = node.findNode("Transform");
    // node.findNode("TransformLink");
    // node.findNode("TransformAssociateModel");

    if (indexNode)
    {
        auto indexData = indexNode->getArrayIntegers(0);
        auto indexDataCount = indexNode->getElementCount(0);
        indexes.resize(indexDataCount);
        for (uint i = 0; i < indexDataCount; i++)
            indexes[i] = indexData[i];
    }

    if (weightNode)
    {
        auto weightData = weightNode->getArrayDouble(0);
        auto weightDataCount = weightNode->getElementCount(0);
        weights.resize(weightDataCount);
        for (uint i = 0; i < weightDataCount; i++)
            weights[i] = static_cast<float>(weightData[i]);
    }

    if (transformNode)
    {
        auto mInvData = transformNode->getArrayDouble(0);
        for (int i = 0; i < 16; i++)
        {
            mInvTransform[i / 4][i % 4] = static_cast<float>(mInvData[i]);
        }
    }
}

std::shared_ptr<Armature> FBXDeformer::getAsSkeleton(FBXGeometry *targetGeometry)
{
    if (!isSkin())
        return nullptr;

    auto retargetVertexList = targetGeometry->getRetargetVertexList();

    std::shared_ptr<Armature> armature = std::make_shared<Armature>(name.c_str());

    for (auto &def : children)
    {
        auto origIndexes = def->getIndexes();
        auto origWeights = def->getWeights();
        uint origAmount = origIndexes.size();

        std::vector<int> newIndexes;
        std::vector<float> newWeights;

        for (uint i = 0; i < origAmount; i++)
        {
            int origIndex = origIndexes[i];
            for (auto &retarget : retargetVertexList)
            {
                if (retarget.from == origIndex)
                {
                    newIndexes.push_back(retarget.to);
                    newWeights.push_back(origWeights[i]);
                }
            }
        }

        armature->addBone(
            def->getName(),
            newIndexes,
            newWeights,
            def->getInvTransform());
    }

    return armature;
}