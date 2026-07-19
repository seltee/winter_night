#include "features/scene/actorTerrain.h"
#include "features/renderer/renderer.h"
#include <cmath>
#include <iostream>

using namespace wne;

ActorTerrain::ActorTerrain(Renderer *renderer, int resolution, float size, float heightFactor, std::shared_ptr<Image> image) : Actor(renderer)
{
    this->renderer = renderer;
    setResolutionSize(resolution, size);
    shapeByImage(image, heightFactor);
    updateMesh();
}

ActorTerrain::~ActorTerrain()
{
    if (mesh)
        mesh->freeObjectId(objectId);
}

void ActorTerrain::setResolution(int resolution)
{
    this->resolution = std::max(resolution, 2);
    updateGrid();
}

void ActorTerrain::setSize(float size)
{
    this->size = std::max(size, 0.1f);
    updateGrid();
}

void ActorTerrain::setResolutionSize(int resolution, float size)
{
    this->resolution = std::max(resolution, 2);
    this->size = std::max(size, 0.1f);
    updateGrid();
}

void ActorTerrain::shapeByImage(std::shared_ptr<Image> image, float heightFactor)
{
    float pickRadius = 1.0f / (float)(resolution);
    float partSize = 1.0f / (float)(resolution - 1);

    for (int iz = 0; iz < resolution; iz++)
    {
        for (int ix = 0; ix < resolution; ix++)
        {
            float height = image->getAvarage((float)ix * partSize, (float)iz * partSize, pickRadius);
            heights[iz * resolution + ix] = height * heightFactor;
        }
    }
}

void ActorTerrain::updateGrid()
{
    int amountOfPoints = resolution * resolution;
    heights.resize(amountOfPoints);
    for (int i = 0; i < amountOfPoints; i++)
    {
        heights[i] = 0.0f;
    }
}

void ActorTerrain::updateMesh()
{
    float halfSize = size * 0.5f;
    float startX = -halfSize;
    float startZ = -halfSize;
    float partSize = 1.0f / (float)(resolution - 1);

    std::vector<wne::VertexTextured> vertices;
    std::vector<uint16_t> indices;

    float shiftX, shiftZ;
    for (int iz = 0; iz < resolution; iz++)
    {
        for (int ix = 0; ix < resolution; ix++)
        {
            Vector3 normal(0, 1.0f, 0);
            if (iz > 0 && iz < resolution - 1 && ix > 0 && ix < resolution - 1)
            {
                float hL = heights[iz * resolution + ix - 1];
                float hR = heights[iz * resolution + ix + 1];
                float hD = heights[(iz - 1) * resolution + ix];
                float hU = heights[(iz + 1) * resolution + ix];
                normal = normalize(Vector3(hL - hR, 1.0f, hD - hU));
            }

            shiftX = (float)ix * partSize;
            shiftZ = (float)iz * partSize;
            vertices.push_back(
                {{startX + shiftX * size, heights[iz * resolution + ix], startZ + shiftZ * size},
                 {shiftX, shiftZ},
                 normal});
        }
    }

    int quadInLine = resolution - 1;
    for (int iz = 0; iz < quadInLine; iz++)
    {
        for (int ix = 0; ix < quadInLine; ix++)
        {
            indices.push_back(iz * resolution + ix);
            indices.push_back((iz + 1) * resolution + ix);
            indices.push_back(iz * resolution + ix + 1);

            indices.push_back(iz * resolution + ix + 1);
            indices.push_back((iz + 1) * resolution + ix);
            indices.push_back((iz + 1) * resolution + ix + 1);
        }
    }

    std::shared_ptr<wne::Model> model = Model::createFromData(vertices, indices);
    mesh = renderer->createMesh(model);

    if (objectId == 0xffffffff)
        objectId = mesh->genNewObjectId();

    boundingRadius = sqrtf(halfSize * halfSize + halfSize * halfSize);
}

void ActorTerrain::setMaterial(std::shared_ptr<Material> material)
{
    this->material = material;
}

float ActorTerrain::getHeightLocal(float x, float y)
{
    // heights resolution
    int xPoint = (int)((float)resolution * x);
    int yPoint = (int)((float)resolution * y);
    uint xPointClamped = std::min(std::max(xPoint, 0), resolution - 1);
    uint yPointClamped = std::min(std::max(yPoint, 0), resolution - 1);

    return heights[yPointClamped * resolution + xPointClamped];
}

float ActorTerrain::getHeightGlobal(float x, float y)
{
    return getHeightLocal((x + size * 0.5f) / size, (y + size * 0.5f) / size);
}

float ActorTerrain::getBoundingRadius()
{
    return boundingRadius;
}

void ActorTerrain::renderDepthShadow(Vector3 &lightPosition)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !mesh || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, false, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorTerrain::renderDepth()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || materialToUse->getColorBlending() != ColorBlending::Solid || !mesh || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorTerrain::renderColor()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !mesh || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights = currentScene->collectAffectingLights(getPosition(), 0.0f);
    materialToUse->bindColor(objectId, lights, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

Actor::RenderPass ActorTerrain::getRenderPass()
{
    return (!material || material->getColorBlending() == ColorBlending::Solid) ? RenderPass::Main : RenderPass::Blended;
}