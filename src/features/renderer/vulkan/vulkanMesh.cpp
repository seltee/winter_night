#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanFrame.h"
#include <iostream>
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

using namespace wne;

VulkanMesh::VulkanMesh(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
    this->vulkanDevice = vulkanUtils->getVulkanDevice();
}

VulkanMesh::~VulkanMesh()
{
    if (vertexBuffer)
        vkDestroyBuffer(vulkanDevice->getDevice(), vertexBuffer, nullptr);
    if (vertexBufferMemory)
        vkFreeMemory(vulkanDevice->getDevice(), vertexBufferMemory, nullptr);
    if (indexBuffer)
        vkDestroyBuffer(vulkanDevice->getDevice(), indexBuffer, nullptr);
    if (indexBufferMemory)
        vkFreeMemory(vulkanDevice->getDevice(), indexBufferMemory, nullptr);
}

std::shared_ptr<VulkanMesh> VulkanMesh::create(std::shared_ptr<Model> model, VulkanUtils *vulkanUtils)
{
    auto mesh = std::make_shared<VulkanMesh>(vulkanUtils);
    if (model->getDataType() == ModelDataType::VertexColoredInd16 || model->getDataType() == ModelDataType::VertexColoredInd32)
    {
        if (model->is32bitIndicides())
        {
            if (mesh->setup(model->getAsVertexColored(), model->getAsIndex32()))
                return mesh;
        }
        else
        {
            if (mesh->setup(model->getAsVertexColored(), model->getAsIndex16()))
                return mesh;
        }
        return nullptr;
    }
    return nullptr;
}

bool VulkanMesh::setup(std::vector<VertexColored> &vertexData, std::vector<uint16> &indexData)
{
    uint64 bufferSize = sizeof(VertexColored) * vertexData.size();
    if (!allocateVertexBuffer(bufferSize, vertexData.data()))
        return false;

    uint64 indexBufferSize = sizeof(uint16) * indexData.size();
    if (!allocateIndexBuffer(indexBufferSize, indexData.data()))
        return false;

    amountOfVerticies = (uint32)vertexData.size();
    amountOfIndices = (uint32)indexData.size();
    amountOfPolygons = amountOfIndices / 3;
    return true;
}

bool VulkanMesh::setup(std::vector<VertexColored> &vertexData, std::vector<uint32> &indexData)
{
    uint64 vertexBufferSize = sizeof(VertexColored) * vertexData.size();
    if (!allocateVertexBuffer(vertexBufferSize, vertexData.data()))
        return false;

    uint64 indexBufferSize = sizeof(uint32) * indexData.size();
    if (!allocateIndexBuffer(indexBufferSize, indexData.data()))
        return false;

    amountOfVerticies = (uint32)vertexData.size();
    amountOfIndices = (uint32)indexData.size();
    amountOfPolygons = amountOfIndices / 3;
    return true;
}

void VulkanMesh::render(void *frameRenderData)
{
    VulkanFrame *vFrame = reinterpret_cast<VulkanFrame *>(frameRenderData);
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    auto commandBuffer = vFrame->getCommandBuffer()->getCommandBuffer();
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    // vkCmdDraw(commandBuffer, amountOfVerticies, 1, 0, 0);
    vkCmdDrawIndexed(commandBuffer, amountOfIndices, 1, 0, 0, 0);
}

bool VulkanMesh::allocateVertexBuffer(uint64 bufferSize, void *data)
{
    // todo don't allocate so many buffers
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!vulkanUtils->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory))
    {
        std::cout << "Error creating buffer" << std::endl;
        return false;
    }

    void *mappedMemory;
    vkMapMemory(vulkanDevice->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &mappedMemory);
    memcpy(mappedMemory, data, (size_t)bufferSize);
    vkUnmapMemory(vulkanDevice->getDevice(), stagingBufferMemory);

    if (!vulkanUtils->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory))
    {
        std::cout << "Error creating local GPU vertex buffer" << std::endl;
        return false;
    }

    vulkanUtils->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(vulkanDevice->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevice->getDevice(), stagingBufferMemory, nullptr);

    return true;
}

bool VulkanMesh::allocateIndexBuffer(uint64 bufferSize, void *data)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vulkanUtils->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void *mappedMemory;
    vkMapMemory(vulkanDevice->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &mappedMemory);
    memcpy(mappedMemory, data, (size_t)bufferSize);
    vkUnmapMemory(vulkanDevice->getDevice(), stagingBufferMemory);

    if (!vulkanUtils->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory))
    {
        std::cout << "Error creating local GPU index buffer" << std::endl;
        return false;
    }

    vulkanUtils->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(vulkanDevice->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevice->getDevice(), stagingBufferMemory, nullptr);

    return true;
}