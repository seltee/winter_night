#include "features/renderer/meshCollection.h"

using namespace wne;

void MeshCollection::addMesh(std::shared_ptr<Mesh> mesh)
{
    meshes.push_back(mesh);
}
