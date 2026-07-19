#pragma once
#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/vulkanInstance.h"
#include "features/renderer/vulkan/vulkanText.h"
#include "core/core.h"
#include <memory>

#define MAX_DEBUG_IDS 128

namespace wne
{
    class WNE_API RendererVulkan : public Renderer
    {
    public:
        void *getFrameData() override final;

        void setSyncState(bool syncEnabled) override final;
        bool getSyncState() override final;

        void update(float delta) override final;
        void renderStart() override final;
        void renderScenes() override final;
        void renderDebug() override final;
        void renderFinish() override final;

        void renderAtmosphereMap(std::shared_ptr<Material> atmoMaterial) override final;
        void provideSceneData(
            const Vector4 &ambientColor,
            const Vector4 &cameraPosition,
            Texture *radianceMap,
            float radienceMapFactor) override final;

        void setMSAASampleCount(uint sampleCount) override final;
        uint getMaxMSAASampleCount() override final;

        std::shared_ptr<wne::Material> getDefaultMaterial() override final;
        std::shared_ptr<Mesh> getDefaultPlane() override final;
        std::shared_ptr<Mesh> getDefaultCube() override final;
        void prepareRenderingState() override final;

        void changeWindowSize(int32 width, int32 height) override final;
        std::shared_ptr<Mesh> createMesh(std::shared_ptr<Model> model) override final;
        std::shared_ptr<MeshCollection> createMeshCollection(std::vector<std::shared_ptr<Model>> models) override final;
        std::shared_ptr<MeshArmature> createMeshArmature(std::shared_ptr<Armature> armature) override final;
        std::shared_ptr<Texture> createTexture(std::shared_ptr<Image> image) override final;
        std::shared_ptr<Text> createText(std::shared_ptr<Font> font) override final;
        std::shared_ptr<MaterialFlat> createFlatMaterial() override final;
        std::shared_ptr<MaterialFlat> createFlatMaterial(std::shared_ptr<Texture> texture) override final;
        std::shared_ptr<Material> createAtmosphereMaterial(std::shared_ptr<Texture> texture) override final;

        std::shared_ptr<LightDirectional> createLightDirectional() override final;
        std::shared_ptr<Light> createLightOmni() override final;
        std::shared_ptr<Light> createLightSpot() override final;

    protected:
        RendererVulkan() = default;
        void setupDefaults();

        std::shared_ptr<MaterialFlat> createDebugMaterial(uint32 color);
        Material *getDebugMaterial(DebugColor color);

        std::unique_ptr<VulkanInstance> instance;
        std::shared_ptr<Mesh> atmoSphere;
        uint64 atmoSphereMeshId = 0xffffffff;

        std::shared_ptr<Material> defaultMaterial;
        std::shared_ptr<Mesh> defaultPlane;
        std::shared_ptr<Mesh> defaultCube;

        std::shared_ptr<MaterialFlat> debugMaterialGreen;
        std::shared_ptr<MaterialFlat> debugMaterialRed;
        std::shared_ptr<MaterialFlat> debugMaterialYellow;
        std::shared_ptr<MaterialFlat> debugMaterialWhite;

        uint MSAASampleCount = 1;

        uint64 debugIds[MAX_DEBUG_IDS];
    };
}
