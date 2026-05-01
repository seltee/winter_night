#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/mesh.h"
#include "features/data/model.h"
#include <memory>
#include <vector>

namespace wne
{
    class VulkanMesh : public Mesh
    {
    public:
        VulkanMesh(VulkanUtils *vulkanUtils);
        ~VulkanMesh();

        static std::shared_ptr<VulkanMesh> create(std::shared_ptr<Model> model, VulkanUtils *vulkanUtils);

        bool setup(std::vector<VertexColored> &data);

        void render(void *frameRenderData) override final;

    protected:
        VulkanDevice *vulkanDevice = nullptr;
        VulkanUtils *vulkanUtils = nullptr;
        VkBuffer vertexBuffer = nullptr;
        VkDeviceMemory vertexBufferMemory = nullptr;
        uint32 amountOfVerticies = 0;
    };
}