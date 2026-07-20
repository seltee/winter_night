#include "features/scene/actorAnimatedMeshHelpers.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"
#include <iterator>
#include <string>

using namespace wne;

AnimatedMeshNode::AnimatedMeshNode(uint64 objectId, const char *name, const char *parentName, bool isEmptyFlag)
{
    this->objectId = objectId;
    this->name = std::string(name);
    this->parentName = std::string(parentName);
    this->isEmptyFlag = isEmptyFlag;
}

AnimatedMeshArmatureNode::AnimatedMeshArmatureNode(Renderer *renderer, std::shared_ptr<Armature> armature, std::vector<AnimatedMeshNode> &nodes)
{
    meshArmature = renderer->createMeshArmature(armature);

    auto bones = armature->getBones();
    bindings.resize(bones.size());

    for (uint i = 0; i < bones.size(); i++)
    {
        for (auto &node : nodes)
        {
            if (!strcmp(node.getName(), bones[i]->getName()))
            {
                bindings[i] = BoneBinding({i,
                                           &node});
                break;
            }
        }
    }
}

void AnimatedMeshArmatureNode::updateMatricies()
{
    for (auto &binding : bindings)
    {
        meshArmature->setBoneTransformationMatrix(binding.matrixIndex, binding.meshNode->transfotmation);
    }
}