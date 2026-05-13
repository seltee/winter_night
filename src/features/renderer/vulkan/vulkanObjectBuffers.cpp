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
    if (objectId < AMOUNT_OF_OBJECTS)
    {
        bufferModelMatricesMapped[objectId] = mModel;
        bufferMVPMatricesMapped[objectId] = mMVP;
        bufferNormalMatricesMapped[objectId] = mNormal;
    }
}

void VulkanObjectBuffers::updateLightData(
    uint32 lightId,
    Light::Type type,
    float affectRadius,
    float cutOff,
    float outerCutOff,
    const Vector4 &position,
    const Vector4 &direction,
    const Vector4 &color)
{
    if (lightId < AMOUNT_OF_LIGHTS)
    {
        bufferLightsDataMapped[lightId].affectRadius = affectRadius;
        bufferLightsDataMapped[lightId].cutOff = cutOff;
        bufferLightsDataMapped[lightId].outerCutOff = outerCutOff;
        bufferLightsDataMapped[lightId].position = position;
        bufferLightsDataMapped[lightId].direction = direction;
        bufferLightsDataMapped[lightId].color = color;
        bufferLightsDataMapped[lightId].enableDirectional = (type == Light::Type::Directional);
        bufferLightsDataMapped[lightId].enableOmni = (type == Light::Type::Omni);
        bufferLightsDataMapped[lightId].enableSpot = (type == Light::Type::Spot);
    }
}

bool VulkanObjectBuffers::setup()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    uint64 matrixBufferSize = getMatrixBufferSize();
    uint64 lightsBufferSize = getLightsBufferSize();
    if (!vulkanUtils->createBuffer(
            matrixBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferModelMatrices,
            bufferModelMatricesMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferModelMatricesMemory, 0, matrixBufferSize, 0, (void **)&bufferModelMatricesMapped);

    if (!vulkanUtils->createBuffer(
            matrixBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferMVPMatrices,
            bufferMVPMatricesMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferMVPMatricesMemory, 0, matrixBufferSize, 0, (void **)&bufferMVPMatricesMapped);

    if (!vulkanUtils->createBuffer(
            matrixBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferNormalMatrices,
            bufferNormalMatricesMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferNormalMatricesMemory, 0, matrixBufferSize, 0, (void **)&bufferNormalMatricesMapped);

    if (!vulkanUtils->createBuffer(
            sizeof(GlobalData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferGlobalData,
            bufferGlobalDataMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferGlobalDataMemory, 0, sizeof(GlobalData), 0, (void **)&bufferGlobalDataMapped);

    if (!vulkanUtils->createBuffer(
            lightsBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferLightsData,
            bufferLightsDataMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferLightsDataMemory, 0, lightsBufferSize, 0, (void **)&bufferLightsDataMapped);

    return true;
}

uint32 VulkanObjectBuffers::getNewObjectId()
{
    for (uint32 i = 0; i < AMOUNT_OF_OBJECTS; i++)
    {
        searchObjectIndex = (searchObjectIndex + 1) % AMOUNT_OF_OBJECTS;
        if (bufferObjectsOccupied[searchObjectIndex] == 0)
        {
            bufferObjectsOccupied[searchObjectIndex] = 1;
            return searchObjectIndex;
        }
    }
    return 0xffffffff;
}

void VulkanObjectBuffers::freeObjectId(uint32 objectId)
{
    if (objectId < AMOUNT_OF_OBJECTS)
        bufferObjectsOccupied[objectId] = 0;
}

void VulkanObjectBuffers::setAmbientColor(Vector4 &ambientColor)
{
    bufferGlobalDataMapped->ambientLightColor = ambientColor;
}

uint32 VulkanObjectBuffers::getNewLightId()
{
    for (uint32 i = 0; i < AMOUNT_OF_LIGHTS; i++)
    {
        searchLightIndex = (searchLightIndex + 1) % AMOUNT_OF_LIGHTS;
        if (bufferLightsOccupied[searchLightIndex] == 0)
        {
            bufferLightsOccupied[searchLightIndex] = 1;
            return searchLightIndex;
        }
    }
    return 0xffffffff;
}

void VulkanObjectBuffers::freeLightId(uint32 lightId)
{
    if (lightId < AMOUNT_OF_LIGHTS)
        bufferLightsOccupied[lightId] = 0;
}