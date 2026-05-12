#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

using namespace wne;

VulkanObjectBuffers::VulkanObjectBuffers(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanObjectBuffers::~VulkanObjectBuffers()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (bufferModelMatrices)
        vkDestroyBuffer(device, bufferModelMatrices, nullptr);
    if (bufferModelMatricesMemory)
        vkFreeMemory(device, bufferModelMatricesMemory, nullptr);
    if (bufferMVPMatrices)
        vkDestroyBuffer(device, bufferMVPMatrices, nullptr);
    if (bufferMVPMatricesMemory)
        vkFreeMemory(device, bufferMVPMatricesMemory, nullptr);
    if (bufferNormalMatrices)
        vkDestroyBuffer(device, bufferNormalMatrices, nullptr);
    if (bufferNormalMatricesMemory)
        vkFreeMemory(device, bufferNormalMatricesMemory, nullptr);
}

void VulkanObjectBuffers::updateObjectData(uint32 objectId, const Matrix4x4 &mModel, const Matrix4x4 &mNormal, const Matrix4x4 &mMVP) noexcept
{
    bufferModelMatricesMapped[objectId] = mModel;
    bufferMVPMatricesMapped[objectId] = mMVP;
    bufferNormalMatricesMapped[objectId] = mNormal;
}

bool VulkanObjectBuffers::setup()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    uint64 size = sizeof(Matrix4x4) * AMOUNT_OF_OBJECTS;
    if (!vulkanUtils->createBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferModelMatrices,
            bufferModelMatricesMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferModelMatricesMemory, 0, size, 0, (void **)&bufferModelMatricesMapped);

    if (!vulkanUtils->createBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferMVPMatrices,
            bufferMVPMatricesMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferMVPMatricesMemory, 0, size, 0, (void **)&bufferMVPMatricesMapped);

    if (!vulkanUtils->createBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferNormalMatrices,
            bufferNormalMatricesMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferNormalMatricesMemory, 0, size, 0, (void **)&bufferNormalMatricesMapped);

    return true;
}

uint32 VulkanObjectBuffers::getNewObjectId()
{
    for (uint32 i = 0; i < AMOUNT_OF_OBJECTS; i++)
    {
        searchIndex = (searchIndex + 1) % AMOUNT_OF_OBJECTS;
        if (bufferOccupied[searchIndex] == 0)
        {
            bufferOccupied[searchIndex] = 1;
            return searchIndex;
        }
    }
    return 0xffffffff;
}

void VulkanObjectBuffers::freeObjectId(uint32 objectId)
{
    if (objectId < AMOUNT_OF_OBJECTS)
        bufferOccupied[objectId] = 0;
}