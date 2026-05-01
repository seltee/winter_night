#include "features/scene/objectMesh.h"
#include "features/renderer/renderer.h"

using namespace wne;

ObjectMesh::ObjectMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = std::move(mesh);
}

std::shared_ptr<ObjectMesh> ObjectMesh::create(std::shared_ptr<Mesh> mesh)
{
    return std::make_shared<ObjectMesh>(std::move(mesh));
}

void ObjectMesh::render(Renderer *renderer)
{
    mesh->render(renderer->getFrameData());
}