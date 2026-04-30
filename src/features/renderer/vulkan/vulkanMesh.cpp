#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanFrame.h"
#include <iostream>
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

using namespace WNE;

VulkanMesh::VulkanMesh(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanMesh::~VulkanMesh()
{
    if (vertexBuffer)
        vkDestroyBuffer(vulkanDevice->getDevice(), vertexBuffer, nullptr);
    if (vertexBufferMemory)
        vkFreeMemory(vulkanDevice->getDevice(), vertexBufferMemory, nullptr);
}

std::shared_ptr<VulkanMesh> VulkanMesh::create(std::shared_ptr<Model> model, VulkanDevice *vulkanDevice)
{
    auto mesh = std::make_shared<VulkanMesh>(vulkanDevice);
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
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(VertexColored) * verticies.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanDevice->getDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
    {
        std::cout << "Error creating buffer" << std::endl;
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanDevice->getDevice(), vertexBuffer, &memRequirements);

    auto memoryType = vulkanDevice->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (memoryType == -1)
    {
        std::cout << "Unable to find supported GPU video memory type" << std::endl;
        return false;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = (uint32_t)memoryType;

    if (vkAllocateMemory(vulkanDevice->getDevice(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
    {
        std::cout << "failed to allocate vertex buffer memory" << std::endl;
        return false;
    }

    vkBindBufferMemory(vulkanDevice->getDevice(), vertexBuffer, vertexBufferMemory, 0);

    // todo maybe move to vkFlushMappedMemoryRanges
    void *mappedMemory;
    vkMapMemory(vulkanDevice->getDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &mappedMemory);
    memcpy(mappedMemory, verticies.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(vulkanDevice->getDevice(), vertexBufferMemory);

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