#pragma once
#include "features/data/model.h"
#include "features/renderer/mesh.h"
#include "core/api.h"
#include <vector>
#include <memory>

namespace wne
{
    class WNE_API MeshCollection
    {
    public:
        void addMesh(std::shared_ptr<Mesh> mesh);

        inline const uint getMeshCount()
        {
            return meshes.size();
        }

        inline const uint64 getNewObjectId() const
        {
            if (meshes.size() > 0)
                return meshes[0]->genNewObjectId();
            return 0xffffffff;
        }

        inline void freeMeshId(uint64 meshId) const
        {
            if (meshes.size() > 0)
                meshes[0]->freeObjectId(meshId);
        }

        std::shared_ptr<Mesh> &operator[](int meshNumber)
        {
            return meshes[meshNumber];
        }

    protected:
        std::vector<std::shared_ptr<Mesh>> meshes;
    };
}