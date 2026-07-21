#pragma once
#include "features/scene/actor.h"
#include "features/data/image.h"
#include "features/data/model.h"
#include "features/renderer/mesh.h"
#include "features/renderer/material.h"
#include "core/api.h"
#include "core/core.h"
#include <memory>
#include <vector>

namespace wne
{
    class Renderer;

    class WNE_API ActorTerrain : public Actor
    {
    public:
        ActorTerrain(Renderer *renderer, int resolution, float size, float heightFactor, std::shared_ptr<Image> image);
        ~ActorTerrain();

        void setResolution(int resolution);
        void setSize(float size);
        void setResolutionSize(int resolution, float size);
        void shapeByImage(std::shared_ptr<Image> image, float heightFactor);
        void updateGrid();
        void updateMesh();

        void setMaterial(std::shared_ptr<Material> material);

        // 0.0 - 1.0
        float getHeightLocal(float x, float y);
        // -size - size
        float getHeightGlobal(float x, float y);

        float getBoundingRadius() override final;

        void renderDepthShadow(Vector3 &lightPosition) override final;
        void renderDepth() override final;
        void renderColor(bool isBlendingPhase) override final;

        bool isBlendingPassRequired() override final;

    protected:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint64 objectId = 0xffffffff;

        std::vector<float> heights;

        int resolution = 10;
        float size = 10.0f;
        float boundingRadius = 1.0f;

        Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    };
}