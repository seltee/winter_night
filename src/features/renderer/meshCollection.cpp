#include "features/renderer/meshCollection.h"

using namespace wne;

MeshCollection::Entity *MeshCollection::addMesh(std::shared_ptr<Mesh> mesh)
{
    entities.emplace_back(Entity({mesh,
                                  std::string(""),
                                  std::string("")}));
    return &entities[entities.size() - 1];
}

MeshCollection::Entity *MeshCollection::addMesh(std::shared_ptr<Mesh> mesh, const char *name)
{
    entities.emplace_back(Entity({mesh,
                                  std::string(name),
                                  std::string("")}));
    return &entities[entities.size() - 1];
}

MeshCollection::Entity *MeshCollection::addMesh(std::shared_ptr<Mesh> mesh, const char *name, const char *parentName)
{
    entities.emplace_back(Entity({mesh,
                                  std::string(name),
                                  std::string(parentName)}));
    return &entities[entities.size() - 1];
}

void MeshCollection::setArmature(const char *targetName, std::shared_ptr<Armature> armature)
{
    this->armatureTargetName = std::string(targetName);
    this->armature = armature;
}
