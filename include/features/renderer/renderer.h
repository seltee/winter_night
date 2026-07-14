#pragma once
#include "features/data/model.h"
#include "features/data/font.h"
#include "features/renderer/mesh.h"
#include "features/renderer/meshCollection.h"
#include "features/renderer/texture.h"
#include "features/renderer/material.h"
#include "features/renderer/materialFlat.h"
#include "features/renderer/rendererState.h"
#include "features/renderer/text.h"
#include "features/scene/scene.h"
#include "features/data/image.h"
#include "core/core.h"
#include "core/math.h"
#include <vector>
#include <mutex>

namespace wne
{
    class Light;
    class LightDirectional;

    class WNE_API Renderer
    {
    public:
        enum class DebugColor
        {
            Red,
            Green,
            Yellow,
            White
        };

        struct DebugLineData
        {
            Vector3 from;
            Vector3 to;
            float removeTimer;
            DebugColor color;
            bool onTop;
        };

        struct DebugCubeData
        {
            Vector3 position;
            float removeTimer;
            DebugColor color;
            bool onTop;
        };

        virtual ~Renderer();
        virtual void update(float delta);
        virtual void renderStart();
        virtual void renderScenes();
        virtual void renderDebug();
        virtual void renderFinish();

        virtual void changeWindowSize(int32 width, int32 height);
        virtual std::shared_ptr<Mesh> createMesh(std::shared_ptr<Model> model);
        virtual std::shared_ptr<MeshCollection> createMeshCollection(std::vector<std::shared_ptr<Model>> models);
        virtual std::shared_ptr<Texture> createTexture(std::shared_ptr<Image> image);
        virtual std::shared_ptr<Text> createText(std::shared_ptr<Font> font);
        virtual std::shared_ptr<MaterialFlat> createFlatMaterial();
        virtual std::shared_ptr<MaterialFlat> createFlatMaterial(std::shared_ptr<Texture> texture);
        virtual std::shared_ptr<Material> createAtmosphereMaterial(std::shared_ptr<Texture> texture);

        virtual void *getFrameData();

        virtual void setSyncState(bool syncEnabled);
        virtual bool getSyncState();

        virtual void renderAtmosphereMap(std::shared_ptr<Material> atmoMaterial);
        virtual void provideSceneData(
            const Vector4 &ambientColor,
            const Vector4 &cameraPosition,
            Texture *radianceMap,
            float radienceMapFactor);

        virtual std::shared_ptr<LightDirectional> createLightDirectional();
        virtual std::shared_ptr<Light> createLightOmni();
        virtual std::shared_ptr<Light> createLightSpot();

        virtual std::shared_ptr<Material> getDefaultMaterial();
        virtual std::shared_ptr<Mesh> getDefaultPlane();

        virtual void prepareRenderingState();

        virtual void setMSAASampleCount(uint sampleCount);
        uint getMSAASampleCount();
        virtual uint getMaxMSAASampleCount();

        std::shared_ptr<Scene> createScene();

        void addDebugLine(const Vector3 &from, const Vector3 &to, DebugColor color, float timer = 0.0f, bool onTop = false);
        void addDebugCube(const Vector3 &position, DebugColor color, float timer = 0.0f, bool onTop = false);
        void updateDebugVisuals(float delta);

        inline RendererState *getState()
        {
            return state;
        }

        void addScene(std::shared_ptr<Scene> scene);

    protected:
        std::vector<std::shared_ptr<Scene>> scenes;
        RendererState *state = nullptr;
        std::mutex mutexDebugData;
        std::vector<DebugLineData> debugLineData;
        std::vector<DebugCubeData> debugCubeData;
    };
}