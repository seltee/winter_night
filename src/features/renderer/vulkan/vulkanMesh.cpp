#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanFrame.h"
#include <iostream>
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

using namespace WNE;

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
}

std::shared_ptr<VulkanMesh> VulkanMesh::create(std::shared_ptr<Model> model, VulkanUtils *vulkanUtils)
{
    auto mesh = std::make_shared<VulkanMesh>(vulkanUtils);
    if (model->getDataType() == ModelDataType::VertexColored)
    {
        if (mesh->setup(model->getAsVertexColored()))
            return mesh;
        return nullptr;
    }
    return nullptr;
}

bool VulkanMesh::setup(std::vector<VertexColored> &verticies)
{
    // todo don't allocate so many buffers
    uint64 bufferSize = sizeof(VertexColored) * verticies.size();

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
    memcpy(mappedMemory, verticies.data(), (size_t)bufferSize);
    vkUnmapMemory(vulkanDevice->getDevice(), stagingBufferMemory);

    if (!vulkanUtils->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory))
    {
        std::cout << "Error creating local GPU buffer" << std::endl;
        return false;
    }

    vulkanUtils->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(vulkanDevice->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevice->getDevice(), stagingBufferMemory, nullptr);

    amountOfVerticies = (uint32)verticies.size();
    return true;
}

void VulkanMesh::render(void *frameRenderData)
{
    VulkanFrame *vFrame = reinterpret_cast<VulkanFrame *>(frameRenderData);
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    auto commandBuffer = vFrame->getCommandBuffer()->getCommandBuffer();
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(commandBuffer, amountOfVerticies, 1, 0, 0);
}
