#pragma once
#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/vulkanInstance.h"
#include "core/core.h"
#include <memory>

namespace wne
{
    class WNE_API RendererVulkanNT : public Renderer
    {
    public:
        static std::unique_ptr<RendererVulkanNT> create(void *hwnd);
        void *getFrameData() override final;
        void setAmbientColor(Vector4 &color) override final;

        void setSyncState(bool syncEnabled) override final;
        bool getSyncState() override final;

        void update(float delta) override final;
        void render() override final;

        void renderAtmosphereMap(std::shared_ptr<Material> atmoMaterial) override final;

        std::shared_ptr<wne::Material> getDefaultMaterial() override final;

        void changeWindowSize(uint32 width, uint32 height) override final;
        std::shared_ptr<Mesh> createMesh(std::shared_ptr<Model> model) override final;
        std::shared_ptr<Texture> createTexture(std::shared_ptr<Image> image) override final;
        std::shared_ptr<Material> createFlatMaterial(std::shared_ptr<Texture> texture) override final;
        std::shared_ptr<Material> createAtmosphereMaterial(std::shared_ptr<Texture> texture) override final;

        std::shared_ptr<LightDirectional> createLightDirectional() override final;
        std::shared_ptr<Light> createLightOmni() override final;
        std::shared_ptr<Light> createLightSpot() override final;

    protected:
        RendererVulkanNT() = default;

        bool setup(void *hWnd);

        void *hWnd = nullptr;
        std::unique_ptr<VulkanInstance> instance;
        std::shared_ptr<Mesh> atmoSphere;
        uint64 atmoSphereMeshId = 0xffffffff;

        std::shared_ptr<wne::Material> defaultMaterial;
    };
}
