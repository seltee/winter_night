#include "features/loaders/fbx/fbxAnimationLayer.h"
#include "features/logger/logger.h"

using namespace wne;

FBXAnimationLayer::FBXAnimationLayer(FBXNode &node)
{
    // node->print();
    id = node.getLong(0);
    const char *charName = node.getString(1);
    name = std::string(charName);
    Logger::log << "Animation Layer " << id << " " << charName << endl;
}