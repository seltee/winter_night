#pragma once
#include "features/data/model.h"
#include "features/data/armature.h"
#include "features/renderer/mesh.h"
#include "core/api.h"
#include <vector>
#include <memory>

namespace wne
{
    class WNE_API MeshCollection
    {
    public:
        struct Entity
        {
            std::shared_ptr<Mesh> mesh;
            std::string name;
            std::string parentName;
        };

        Entity *addMesh(std::shared_ptr<Mesh> mesh);
        Entity *addMesh(std::shared_ptr<Mesh> mesh, const char *name);
        Entity *addMesh(std::shared_ptr<Mesh> mesh, const char *name, const char *parentName);

        void addArmature(std::shared_ptr<Armature> armature);

        inline const uint getMeshCount()
        {
            return entities.size();
        }

        inline const uint64 getNewObjectId() const
        {
            if (entities.size() > 0)
                return entities[0].mesh->genNewObjectId();
            return 0xffffffff;
        }

        inline void freeMeshId(uint64 meshId) const
        {
            if (entities.size() > 0)
                entities[0].mesh->freeObjectId(meshId);
        }

        inline bool hasArmatures()
        {
            return armatures.size() > 0;
        }

        inline const std::vector<std::shared_ptr<Armature>> &getArmatures()
        {
            return armatures;
        }

        inline Entity &operator[](int meshNumber)
        {
            return entities[meshNumber];
        }

    protected:
        std::vector<Entity> entities;
        std::vector<std::shared_ptr<Armature>> armatures;
    };
}