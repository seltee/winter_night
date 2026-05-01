#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/mesh.h"
#include "features/data/model.h"
#include "core/core.h"
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

        bool setup(std::vector<VertexColored> &vertexData, std::vector<uint16> &indexData);
        bool setup(std::vector<VertexColored> &vertexData, std::vector<uint32> &indexData);

        void render(void *frameRenderData) override final;

        inline uint32 getAmountOfVerticies()
        {
            return amountOfVerticies;
        }

        inline uint32 getAmountOfIndices()
        {
            return amountOfIndices;
        }

        inline uint32 getAmountOfPolygons()
        {
            return amountOfPolygons;
        }

    protected:
        bool allocateVertexBuffer(uint64 bufferSize, void *data);
        bool allocateIndexBuffer(uint64 bufferSize, void *data);

        uint32 amountOfVerticies = 0;
        uint32 amountOfIndices = 0;
        uint32 amountOfPolygons = 0;
        VulkanDevice *vulkanDevice = nullptr;
        VulkanUtils *vulkanUtils = nullptr;
        VkBuffer vertexBuffer = nullptr;
        VkDeviceMemory vertexBufferMemory = nullptr;
        VkBuffer indexBuffer = nullptr;
        VkDeviceMemory indexBufferMemory = nullptr;
    };
}