#include "features/loaders/loader3d.h"
#include "features/loaders/fbx/fbx.h"
#include <iostream>

using namespace wne;

Loader3d::Loader3d()
{
}

std::shared_ptr<Model> Loader3d::loadAsModel(const char *path)
{
    auto modelBase = FBX::loadFile(path);
    if (!modelBase)
        return nullptr;
    return modelBase->getAsModel();
}

std::vector<std::shared_ptr<Model>> Loader3d::loadAsModels(const char *path)
{
    auto modelBase = FBX::loadFile(path);
    if (!modelBase)
        return {};
    return modelBase->getModels();
}

Loader3d::ModelAnimations Loader3d::loadAsModelAnimation(const char *path)
{
    auto modelBase = FBX::loadFile(path);
    if (!modelBase)
        return {};
    return {
        modelBase->getModels(),
        modelBase->getAnimations(),
    };
}