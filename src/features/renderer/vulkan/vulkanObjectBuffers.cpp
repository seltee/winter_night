#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <memory>

using namespace wne;

VulkanObjectBuffers::VulkanObjectBuffers(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanObjectBuffers::~VulkanObjectBuffers()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    for (const auto &buffer : bufferModelMatrices)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferModelMatricesMemory)
        vkFreeMemory(device, buffer, nullptr);

    for (const auto &buffer : bufferMVPMatrices)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferMVPMatricesMemory)
        vkFreeMemory(device, buffer, nullptr);

    for (const auto &buffer : bufferNormalMatrices)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferNormalMatricesMemory)
        vkFreeMemory(device, buffer, nullptr);

    for (const auto &buffer : bufferGlobalData)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferGlobalDataMemory)
        vkFreeMemory(device, buffer, nullptr);

    for (const auto &buffer : bufferLightMVPsData)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferLightMVPsMemory)
        vkFreeMemory(device, buffer, nullptr);

    for (const auto &buffer : bufferLightsData)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferLightsDataMemory)
        vkFreeMemory(device, buffer, nullptr);
}

void VulkanObjectBuffers::updateObjectData(uint32 objectId, const Matrix4x4 &mModel, const Matrix4x4 &mNormal, const Matrix4x4 &mMVP) noexcept
{
    if (objectId < AMOUNT_OF_OBJECTS)
    {
        bufferModelMatricesMapped[currentInFlight][objectId] = mModel;
        bufferMVPMatricesMapped[currentInFlight][objectId] = mMVP;
        bufferNormalMatricesMapped[currentInFlight][objectId] = mNormal;
    }
}

void VulkanObjectBuffers::updateLightData(
    uint32 lightId,
    Light::Type type,
    float affectRadius,
    float cutOff,
    float outerCutOff,
    uint32 shadowId,
    uint32 amountOfCascades,
    float texelSize,
    const Vector4 &position,
    const Vector4 &direction,
    const Vector4 &color)
{
    if (lightId < AMOUNT_OF_LIGHTS)
    {
        auto dataMapped = &bufferLightsDataMapped[currentInFlight][lightId];
        dataMapped->affectRadius = affectRadius;
        dataMapped->cutOff = cutOff;
        dataMapped->outerCutOff = outerCutOff;
        dataMapped->position = position;
        dataMapped->direction = direction;
        dataMapped->color = color;
        dataMapped->shadowId = shadowId;
        dataMapped->amountOfCascades = amountOfCascades;
        dataMapped->texelSize = texelSize;
        dataMapped->enableDirectional = (type == Light::Type::Directional);
        dataMapped->enableOmni = (type == Light::Type::Omni);
        dataMapped->enableSpot = (type == Light::Type::Spot);
    }
}

void VulkanObjectBuffers::updateLightShadowData(
    uint32 shadowId,
    Matrix4x4 &mLightMVP)
{
    if (shadowId < MAX_LIGHT_SHADOWS)
    {
        bufferLightMVPsMapped[currentInFlight][shadowId] = mLightMVP;
    }
}

bool VulkanObjectBuffers::setup(uint maxFramesInFlight)
{
    this->maxFramesInFlight = maxFramesInFlight;
    this->currentInFlight = 0;

    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    uint64 matrixBufferSize = getMatrixBufferSize();
    uint64 lightsBufferSize = getLightsBufferSize();
    uint64 lightMVPsSize = getLightMVPsBufferSize();

    bufferModelMatrices.resize(maxFramesInFlight);
    bufferModelMatricesMemory.resize(maxFramesInFlight);
    bufferModelMatricesMapped.resize(maxFramesInFlight);

    bufferMVPMatrices.resize(maxFramesInFlight);
    bufferMVPMatricesMemory.resize(maxFramesInFlight);
    bufferMVPMatricesMapped.resize(maxFramesInFlight);

    bufferNormalMatrices.resize(maxFramesInFlight);
    bufferNormalMatricesMemory.resize(maxFramesInFlight);
    bufferNormalMatricesMapped.resize(maxFramesInFlight);

    bufferGlobalData.resize(maxFramesInFlight);
    bufferGlobalDataMemory.resize(maxFramesInFlight);
    bufferGlobalDataMapped.resize(maxFramesInFlight);

    bufferLightMVPsData.resize(maxFramesInFlight);
    bufferLightMVPsMemory.resize(maxFramesInFlight);
    bufferLightMVPsMapped.resize(maxFramesInFlight);

    bufferLightsData.resize(maxFramesInFlight);
    bufferLightsDataMemory.resize(maxFramesInFlight);
    bufferLightsDataMapped.resize(maxFramesInFlight);

    for (uint i = 0; i < maxFramesInFlight; i++)
    {
        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferModelMatrices[i],
                bufferModelMatricesMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferModelMatricesMemory[i], 0, matrixBufferSize, 0, (void **)&bufferModelMatricesMapped[i]);

        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferMVPMatrices[i],
                bufferMVPMatricesMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferMVPMatricesMemory[i], 0, matrixBufferSize, 0, (void **)&bufferMVPMatricesMapped[i]);

        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferNormalMatrices[i],
                bufferNormalMatricesMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferNormalMatricesMemory[i], 0, matrixBufferSize, 0, (void **)&bufferNormalMatricesMapped[i]);

        if (!vulkanUtils->createBuffer(
                sizeof(GlobalData),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferGlobalData[i],
                bufferGlobalDataMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferGlobalDataMemory[i], 0, sizeof(GlobalData), 0, (void **)&bufferGlobalDataMapped[i]);

        if (!vulkanUtils->createBuffer(
                lightMVPsSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferLightMVPsData[i],
                bufferLightMVPsMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferLightMVPsMemory[i], 0, lightMVPsSize, 0, (void **)&bufferLightMVPsMapped[i]);

        if (!vulkanUtils->createBuffer(
                lightsBufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferLightsData[i],
                bufferLightsDataMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferLightsDataMemory[i], 0, lightsBufferSize, 0, (void **)&bufferLightsDataMapped[i]);
    }

    dummyBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils);
    if (!dummyBuffer->setup(8, 8, 1, true))
    {
        return false;
    }
    dummyBuffer->transitionToDefined();

    return true;
}

void VulkanObjectBuffers::swap()
{
    currentInFlight = (currentInFlight + 1) % maxFramesInFlight;
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

void VulkanObjectBuffers::setGlobalData(
    const Vector4 &ambientColor,
    const Vector4 &cameraPosition,
    uint useRadianceMap,
    float radienceMapFactor)
{
    bufferGlobalDataMapped[currentInFlight]->ambientLightColor = ambientColor;
    bufferGlobalDataMapped[currentInFlight]->cameraPosition = cameraPosition;
    bufferGlobalDataMapped[currentInFlight]->useRadianceMap = useRadianceMap;
    bufferGlobalDataMapped[currentInFlight]->radienceMapFactor = radienceMapFactor;
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