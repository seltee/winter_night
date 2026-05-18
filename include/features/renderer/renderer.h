#pragma once
#include "features/data/model.h"
#include "features/renderer/mesh.h"
#include "features/renderer/texture.h"
#include "features/renderer/material.h"
#include "features/renderer/rendererState.h"
#include "features/scene/scene.h"
#include "features/data/image.h"
#include "core/core.h"
#include "core/math.h"
#include <vector>

namespace wne
{
    class Light;
    class LightDirectional;

    class WNE_API Renderer
    {
    public:
        virtual ~Renderer();
        virtual void update(float delta);
        virtual void render();

        virtual void changeWindowSize(uint32 width, uint32 height);
        virtual std::shared_ptr<Mesh> createMesh(std::shared_ptr<Model> model);
        virtual std::shared_ptr<Texture> createTexture(std::shared_ptr<Image> image);
        virtual std::shared_ptr<Material> createFlatMaterial(std::shared_ptr<Texture> texture);
        virtual std::shared_ptr<Material> createAtmosphereMaterial(std::shared_ptr<Texture> texture);

        virtual void *getFrameData();
        virtual void setAmbientColor(Vector4 &color);

        virtual void setSyncState(bool syncEnabled);
        virtual bool getSyncState();

        virtual void renderAtmosphereMap(std::shared_ptr<Material> atmoMaterial);

        virtual std::shared_ptr<LightDirectional> createLightDirectional();
        virtual std::shared_ptr<Light> createLightOmni();
        virtual std::shared_ptr<Light> createLightSpot();

        virtual std::shared_ptr<wne::Material> getDefaultMaterial();

        inline RendererState *getState()
        {
            return state;
        }

        void addScene(std::shared_ptr<Scene> scene);

    protected:
        std::vector<std::shared_ptr<Scene>> scenes;
        RendererState *state = nullptr;
    };
}