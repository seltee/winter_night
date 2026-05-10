#pragma once
#include "features/loaders/fbx/fbxNode.h"
#include "features/loaders/fbx/fbxGeometry.h"
#include "features/loaders/fbx/fbxModel.h"
#include "features/data/base3d.h"
#include "features/data/file.h"
#include <memory>

namespace wne
{
    class FBX
    {
    public:
        static std::shared_ptr<Base3d> loadFile(const char *path);

    protected:
        FBX();
        static FBXNode *findByName(std::vector<std::unique_ptr<wne::FBXNode>> &list, const char *name);
        static FBXGeometry *getGeometryById(std::vector<FBXGeometry> &list, uint64 id);
        static FBXModel *getModelById(std::vector<FBXModel> &list, uint64 id);
    };
};
