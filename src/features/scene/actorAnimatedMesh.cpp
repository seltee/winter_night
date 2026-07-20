#include "features/scene/actorAnimatedMesh.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"
#include <iterator>
#include <string>

using namespace wne;

ActorAnimatedMesh::ActorAnimatedMesh(Renderer *renderer, std::shared_ptr<MeshCollection> mesh) : Actor(renderer)
{
    Logger::log << "Animated mesh setup" << endl;
    this->mesh = mesh;
    count = mesh->getMeshCount();

    // adding nodes
    nodes.reserve(count);
    for (uint i = 0; i < count; i++)
    {
        nodes.emplace_back(AnimatedMeshNode(
            mesh->getNewObjectId(),
            (*mesh)[i].name.c_str(),
            (*mesh)[i].parentName.c_str(),
            (*mesh)[i].mesh->isEmpty()));
    }

    // adding armature
    auto armature = mesh->getArmature();
    if (armature)
    {
        auto animatedMeshArmatureNode = std::make_shared<AnimatedMeshArmatureNode>(renderer, armature, nodes);
        auto targetMesh = getMeshNodeByName(mesh->getArmatureTargetName());
        if (targetMesh)
            targetMesh->armature = animatedMeshArmatureNode;
        else
            Logger::log << "Unable to find armature mesh" << endl;
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

    Vector4 actorPosition4 = getModelMatrix() * Vector4(0, 0, 0, 1.0f);
    actorPosition4 = actorPosition4 / actorPosition4.w;
    Vector3 actorPosition = actorPosition4.xyz();
    float boundingRadius = 0.0f;
    for (uint i = 0; i < count; i++)
    {
        if (nodes[i].isTransformationDirty)
        {
            // main transformation
            Matrix4x4 transformation = getNodeTransformation(&nodes[i]);
            nodes[i].transfotmation = transformation;

            // boundings
            Vector3 scale = extractScale(transformation);
            float nodeRadius = getHighestAxisValue(scale) * (*mesh)[i].mesh->getBoundingRadius();
            nodes[i].boundingRadius = nodeRadius;
            Vector4 position4 = transformation * Vector4(0, 0, 0, 1.0f);
            position4 = position4 / position4.w;
            Vector3 position = position4.xyz();
            nodes[i].position = position;

            boundingRadius = std::max(
                boundingRadius,
                length(position - actorPosition) + nodeRadius);
        }

        if (nodes[i].armature)
            nodes[i].armature->updateMatricies();
    }
    this->boundingRadius = boundingRadius;
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

void ActorAnimatedMesh::updateRenderFlag(ActorCamera *camera)
{
    if (camera)
    {
        Vector4 position = getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        position = position / position.w;
        isInRenderFlag = camera->checkFrustrum(position.xyz(), getBoundingRadius());

        for (uint i = 0; i < count; i++)
            nodes[i].isInRender = !nodes[i].isEmpty() && camera->checkFrustrum(nodes[i].position, nodes[i].boundingRadius);
    }
    else
    {
        isInRenderFlag = false;
    }
}

void ActorAnimatedMesh::renderDepthShadow(Vector3 &lightPosition)
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};

    for (uint i = 0; i < count; i++)
    {
        MeshArmature *meshArmature = nodes[i].armature ? nodes[i].armature->meshArmature.get() : nullptr;
        Material *materialToUse = nodes[i].material ? nodes[i].material.get() : renderer->getDefaultMaterial().get();
        materialToUse->bindDepthShadow(
            nodes[i].getObjectId(),
            renderer,
            state->getViewProjectionMatrix() * nodes[i].transfotmation,
            getNormalMatrix(),
            uvModifier,
            meshArmature,
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
        if (nodes[i].isInRender)
        {
            MeshArmature *meshArmature = nodes[i].armature ? nodes[i].armature->meshArmature.get() : nullptr;
            Material *materialToUse = nodes[i].material ? nodes[i].material.get() : renderer->getDefaultMaterial().get();
            materialToUse->bindDepth(
                nodes[i].getObjectId(),
                state->getViewProjectionMatrix() * nodes[i].transfotmation,
                nodes[i].transfotmation,
                getNormalMatrix(),
                uvModifier,
                meshArmature,
                (*mesh)[i].mesh->getDataType());
            (*mesh)[i].mesh->render(renderer->getFrameData());
        }
    }
}

void ActorAnimatedMesh::renderColor()
{
    auto state = renderer->getState();
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    AffectingLights lights = currentScene->collectAffectingLights(getPosition(), 0.0f);

    for (uint i = 0; i < count; i++)
    {
        if (nodes[i].isInRender)
        {
            MeshArmature *meshArmature = nodes[i].armature ? nodes[i].armature->meshArmature.get() : nullptr;
            Material *materialToUse = nodes[i].material ? nodes[i].material.get() : renderer->getDefaultMaterial().get();
            materialToUse->bindColor(
                nodes[i].getObjectId(),
                lights,
                state->getViewProjectionMatrix() * nodes[i].transfotmation,
                nodes[i].transfotmation,
                getNormalMatrix(),
                uvModifier,
                meshArmature,
                (*mesh)[i].mesh->getDataType());
            (*mesh)[i].mesh->render(renderer->getFrameData());
        }
    }

    if (debugViewChierarchyState)
    {
        for (uint i = 0; i < count; i++)
        {
            if (nodes[i].parentNode)
            {
                Vector4 pFrom4 = nodes[i].transfotmation * Vector4(0, 0, 0, 1.0f);
                pFrom4 = pFrom4 / pFrom4.w;
                Vector4 pTo4 = nodes[i].parentNode->transfotmation * Vector4(0, 0, 0, 1.0f);
                pTo4 = pTo4 / pTo4.w;
                renderer->addDebugLine(pFrom4.xyz(), pTo4.xyz(), Renderer::DebugColor::White);
            }
        }
    }
}

Actor::RenderPass ActorAnimatedMesh::getRenderPass()
{
    return RenderPass::Blended;
}

float ActorAnimatedMesh::getBoundingRadius()
{
    return boundingRadius;
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