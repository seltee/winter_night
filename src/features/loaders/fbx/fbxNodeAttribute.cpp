#include "features/loaders/fbx/fbxNodeAttribute.h"
#include "features/logger/logger.h"

using namespace wne;

FBXNodeAttribute::FBXNodeAttribute(FBXNode &node)
{
    // Logger::log << "FBC Node Attribute" << endl;
    id = node.getLong(0);
    name = node.getString(1);
}