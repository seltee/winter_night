#include "features/scene/actorTerrain.h"
#include "features/renderer/renderer.h"
#include <cmath>

using namespace wne;

ActorTerrain::ActorTerrain(Renderer *renderer)
{
    this->renderer = renderer;
}

std::shared_ptr<ActorTerrain> ActorTerrain::createFromImage(int resolution, float size, float heightFactor, std::shared_ptr<Image> image, Renderer *renderer)
{
    std::shared_ptr<ActorTerrain> ptr = std::make_shared<ActorTerrain>(renderer);
    ptr->setResolutionSize(resolution, size);
    ptr->shapeByImage(image, heightFactor);
    ptr->updateMesh();
    return ptr;
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
    float startX = -size * 0.5f;
    float startZ = -size * 0.5f;
    float partSize = 1.0f / (float)(resolution - 1);

    std::vector<wne::VertexTextured> vertices;
    std::vector<uint16_t> indices;

    float shiftX, shiftZ;
    for (int iz = 0; iz < resolution; iz++)
    {
        for (int ix = 0; ix < resolution; ix++)
        {
            shiftX = (float)ix * partSize;
            shiftZ = (float)iz * partSize;
            vertices.push_back(
                {{startX + shiftX * size, heights[iz * resolution + ix], startZ + shiftZ * size},
                 {shiftX, shiftZ}});
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
}

void ActorTerrain::setMaterial(std::shared_ptr<Material> material)
{
    this->material = material;
}

void ActorTerrain::render(Renderer *renderer)
{
    if (!material || !mesh)
        return;
    material->bind(renderer->getViewProjectionMatrix() * getModelMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}