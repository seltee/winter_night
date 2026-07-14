#include "features/scene/actorAnimatedMesh.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"
#include <iterator>
#include <string>

using namespace wne;

ActorAnimatedMesh::ActorAnimatedMesh(Renderer *renderer, std::shared_ptr<MeshCollection> mesh) : Actor(renderer)
{
    this->mesh = mesh;
    count = mesh->getMeshCount();

    // adding nodes
    nodes.reserve(count);
    for (uint i = 0; i < count; i++)
    {
        nodes.emplace_back(AnimatedMeshNode(
            mesh->getNewObjectId(),
            (*mesh)[i].name.c_str(),
            (*mesh)[i].parentName.c_str()));
    }

    // linking parents
    for (uint i = 0; i < count; i++)
    {
        if (!nodes[i].getParentName() || strlen(nodes[i].getParentName()) == 0)
            continue;
        nodes[i].parentNode = getMeshNodeByName(nodes[i].getParentName());
    }
}

ActorAnimatedMesh::~ActorAnimatedMesh()
{
    for (auto &node : nodes)
    {
        mesh->freeMeshId(node.getObjectId());
    }
}

void ActorAnimatedMesh::setMaterialByName(const char *name, std::shared_ptr<Material> material)
{
    for (auto &node : nodes)
    {
        if (!strcmp(node.getName(), name))
            node.material = material;
    }
}

void ActorAnimatedMesh::setMaterialToAll(std::shared_ptr<Material> material)
{
    for (auto &node : nodes)
        node.material = material;
}

void ActorAnimatedMesh::update(float delta)
{
    for (auto &node : nodes)
        node.isTransformationDirty = true;

    for (auto &track : tracks)
        track->update(delta);

    for (uint i = 0; i < count; i++)
    {
        if (nodes[i].isTransformationDirty)
            nodes[i].transfotmation = getNodeTransformation(&nodes[i]);
    }
}

Matrix4x4 ActorAnimatedMesh::getNodeTransformation(AnimatedMeshNode *node)
{
    if (!node->isTransformationDirty)
        return node->transfotmation;

    node->isTransformationDirty = false;

    Matrix4x4 base = node->parentNode ? getNodeTransformation(node->parentNode) : getModelMatrix();
    Matrix4x4 out = Matrix4x4::identity();

    for (auto &track : tracks)
        out = out * track->getTransformationMatrix(node->getName(), 1.0f);

    node->transfotmation = base * out;
    return node->transfotmation;
}

void ActorAnimatedMesh::renderDepthShadow(Vector3 &lightPosition)
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};

    for (uint i = 0; i < count; i++)
    {
        Material *materialToUse = nodes[i].material ? nodes[i].material.get() : renderer->getDefaultMaterial().get();
        materialToUse->bindDepthShadow(
            nodes[i].getObjectId(),
            renderer,
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            getNormalMatrix(),
            uvModifier,
            false,
            (*mesh)[i].mesh->getDataType());
        (*mesh)[i].mesh->render(renderer->getFrameData());
    }
}

void ActorAnimatedMesh::renderDepth()
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};

    for (uint i = 0; i < count; i++)
    {
        Material *materialToUse = nodes[i].material ? nodes[i].material.get() : renderer->getDefaultMaterial().get();
        materialToUse->bindDepth(
            nodes[i].getObjectId(),
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            nodes[i].transfotmation,
            getNormalMatrix(),
            uvModifier,
            (*mesh)[i].mesh->getDataType());
        (*mesh)[i].mesh->render(renderer->getFrameData());
    }
}

void ActorAnimatedMesh::renderColor()
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    AffectingLights lights = currentScene->collectAffectingLights(getPosition(), 0.0f);

    for (uint i = 0; i < count; i++)
    {
        Material *materialToUse = nodes[i].material ? nodes[i].material.get() : renderer->getDefaultMaterial().get();
        materialToUse->bindColor(
            nodes[i].getObjectId(),
            lights,
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            nodes[i].transfotmation,
            getNormalMatrix(),
            uvModifier,
            (*mesh)[i].mesh->getDataType());
        (*mesh)[i].mesh->render(renderer->getFrameData());
    }
}

Actor::RenderPass ActorAnimatedMesh::getRenderPass()
{
    return RenderPass::Main;
}

float ActorAnimatedMesh::getBoundingRadius()
{
    return 4.0f;
}

std::shared_ptr<Animation3dTrack> ActorAnimatedMesh::createAnimationTrack(std::shared_ptr<Animation3d> animation)
{

    auto newTrack = std::make_shared<Animation3dTrack>(animation);
    tracks.push_back(newTrack);
    return newTrack;
}

void ActorAnimatedMesh::removeAnimationTrack(std::shared_ptr<Animation3dTrack> track)
{
    std::erase_if(tracks, [&](std::shared_ptr<Animation3dTrack> &checkTrack)
                  { return track.get() == checkTrack.get(); });
}