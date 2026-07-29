#include "features/scene/actorMesh.h"
#include "features/renderer/renderer.h"

using namespace wne;

ActorMesh::ActorMesh(Renderer *renderer, std::shared_ptr<Mesh> mesh) : Actor(renderer)
{
    this->mesh = std::move(mesh);
    objectId = this->mesh->genNewObjectId();
}

ActorMesh::~ActorMesh()
{
    this->mesh->freeObjectId(objectId);
}

void ActorMesh::setMaterial(std::shared_ptr<Material> material)
{
    this->material = std::move(material);
}

void ActorMesh::renderDepthShadow(Vector3 &lightPosition)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, false, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorMesh::renderDepth()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || materialToUse->getColorBlending() != ColorBlending::Solid || !currentScene || objectId == 0xffffffff)
        return;

    if (materialToUse->getColorBlending() == ColorBlending::Solid)
    {
        auto state = renderer->getState();
        materialToUse->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, mesh->getDataType());
        mesh->render(renderer->getFrameData());
    }
}

void ActorMesh::renderColor(bool isBlendingPhase)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    bool isMainPhase = materialToUse->getColorBlending() == ColorBlending::Solid;
    if ((isBlendingPhase && !isMainPhase) || (!isBlendingPhase && isMainPhase))
    {
        auto state = renderer->getState();
        AffectingLights lights = currentScene->collectAffectingLights(getPosition(), 0.0f);
        Texture *radience = currentScene ? currentScene->getAtmosphereRadienceTexture().get() : nullptr;
        materialToUse->bindColor(objectId,
                                 lights,
                                 state->getViewProjectionMatrix() * getModelMatrix(),
                                 getModelMatrix(),
                                 getNormalMatrix(),
                                 uvModifier,
                                 nullptr, radience,
                                 mesh->getDataType());
        mesh->render(renderer->getFrameData());
    }
}

bool ActorMesh::isBlendingPassRequired()
{
    return material && material->getColorBlending() != ColorBlending::Solid;
}

float ActorMesh::getBoundingRadius()
{
    float size = getHighestAxisValue(scale);
    return mesh->getBoundingRadius() * size;
}
