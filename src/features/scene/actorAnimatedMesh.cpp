#include "features/scene/actorAnimatedMesh.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"
#include <iterator>

using namespace wne;

ActorAnimatedMesh::ActorAnimatedMesh(Renderer *renderer, std::shared_ptr<MeshCollection> mesh) : Actor(renderer)
{
    this->mesh = mesh;
    count = mesh->getMeshCount();
    nodes.resize(count);
    for (uint i = 0; i < count; i++)
    {
        nodes[i].objectId = mesh->getNewObjectId();
        nodes[i].material = nullptr;
    }
}

ActorAnimatedMesh::~ActorAnimatedMesh()
{
    for (auto &node : nodes)
        mesh->freeMeshId(node.objectId);
}

void ActorAnimatedMesh::update(float delta)
{
    for (uint i = 0; i < count; i++)
    {
        Matrix4x4 out = Matrix4x4::identity();

        float mixFactorAcc = 0.0f;
        for (auto &track : tracks)
        {
            track->update(delta);
            mixFactorAcc += track->getMixFactor();
        }

        if (mixFactorAcc > 0)
        {
            for (auto &track : tracks)
                out = out * track->getTransformationMatrix((*mesh)[i]->getName(), mixFactorAcc);
        }

        nodes[i].transfotmation = getModelMatrix() * out;
    }
}

void ActorAnimatedMesh::renderDepthShadow(Vector3 &lightPosition)
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};

    for (uint i = 0; i < count; i++)
    {
        Material *materialToUse = renderer->getDefaultMaterial().get();
        materialToUse->bindDepthShadow(
            nodes[i].objectId,
            renderer,
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            getNormalMatrix(),
            uvModifier,
            false,
            (*mesh)[i]->getDataType());
        (*mesh)[i]->render(renderer->getFrameData());
    }
}

void ActorAnimatedMesh::renderDepth()
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};

    for (uint i = 0; i < count; i++)
    {
        Material *materialToUse = renderer->getDefaultMaterial().get();
        materialToUse->bindDepth(
            nodes[i].objectId,
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            getModelMatrix(),
            getNormalMatrix(),
            uvModifier,
            (*mesh)[i]->getDataType());
        (*mesh)[i]->render(renderer->getFrameData());
    }
}

void ActorAnimatedMesh::renderColor()
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    AffectingLights lights = currentScene->collectAffectingLights(getPosition(), 0.0f);

    for (uint i = 0; i < count; i++)
    {
        Material *materialToUse = renderer->getDefaultMaterial().get();
        materialToUse->bindColor(
            nodes[i].objectId,
            lights,
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            getModelMatrix(),
            getNormalMatrix(),
            uvModifier,
            (*mesh)[i]->getDataType());
        (*mesh)[i]->render(renderer->getFrameData());
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

    auto newTrack = std::make_shared<Animation3dTrack>(
        std::vector<std::shared_ptr<Animation3d>>({animation}));
    tracks.push_back(newTrack);
    return newTrack;
}

std::shared_ptr<Animation3dTrack> ActorAnimatedMesh::createAnimationTrack(
    std::vector<std::shared_ptr<Animation3d>> animations)
{
    auto newTrack = std::make_shared<Animation3dTrack>(animations);
    tracks.push_back(newTrack);
    return newTrack;
}

void ActorAnimatedMesh::removeAnimationTrack(std::shared_ptr<Animation3dTrack> track)
{
    std::erase_if(tracks, [&](std::shared_ptr<Animation3dTrack> &checkTrack)
                  { return track.get() == checkTrack.get(); });
}