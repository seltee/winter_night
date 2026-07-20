#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
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

    for (const auto &buffer : bufferBonesData)
        vkDestroyBuffer(device, buffer, nullptr);
    for (const auto &buffer : bufferBonesMemory)
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
    uint64 bonesBufferSize = getBonesBufferSize();
    uint64 boneWeightsBufferSize = getBoneWeightsBufferSize();

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

    bufferBonesData.resize(maxFramesInFlight);
    bufferBonesMemory.resize(maxFramesInFlight);
    bufferBonesMapped.resize(maxFramesInFlight);

    // setting up per frames buffers
    for (uint i = 0; i < maxFramesInFlight; i++)
    {
        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferModelMatrices[i],
                bufferModelMatricesMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferModelMatricesMemory[i], 0, matrixBufferSize, 0, (void **)&bufferModelMatricesMapped[i]);

        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferMVPMatrices[i],
                bufferMVPMatricesMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferMVPMatricesMemory[i], 0, matrixBufferSize, 0, (void **)&bufferMVPMatricesMapped[i]);

        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
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

        if (!vulkanUtils->createBuffer(
                bonesBufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                bufferBonesData[i],
                bufferBonesMemory[i]))
        {
            return false;
        }
        vkMapMemory(device, bufferBonesMemory[i], 0, bonesBufferSize, 0, (void **)&bufferBonesMapped[i]);
    }

    // shared between frames
    if (!vulkanUtils->createBuffer(
            boneWeightsBufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferBoneWeightsData,
            bufferBoneWeightsMemory))
    {
        return false;
    }
    vkMapMemory(device, bufferBoneWeightsMemory, 0, boneWeightsBufferSize, 0, (void **)&bufferBoneWeightsMapped);

    // setting up buffers
    for (int i = 0; i < MAX_BONES_IN_SCENE; i++)
    {
        bufferBonesOccupation[i] = 0xffffffff;
        for (uint f = 0; f < maxFramesInFlight; f++)
            bufferBonesMapped[f][i] = Matrix4x4::identity();
    }
    for (int i = 0; i < MAX_BONE_BINDINGS; i++)
    {
        bufferBoneWeightsOccupation[i] = 0xffffffff;
        for (int r = 0; r < MAX_BONE_WEIGHTS; r++){
            bufferBoneWeightsMapped[i].boneIndex[r] = 0;
            bufferBoneWeightsMapped[i].boneWeight[r] = 0.0f;
        }
    }

    // dummies
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

// returns starting bone index
int32 VulkanObjectBuffers::allocateBonesForObject(uint32 bonesAmount)
{
    static uint32 cId = 0;
    static uint32 lastSearchPoint = 0;
    uint32 searchPoint = lastSearchPoint;

    while (true)
    {
        if (searchPoint + bonesAmount >= MAX_BONES_IN_SCENE)
            searchPoint = 0;
        if (bufferBonesOccupation[searchPoint] == 0xffffffff)
        {
            // free block;
            bool found = true;
            for (uint32 b = 0; b < bonesAmount; b++)
            {
                if (bufferBonesOccupation[searchPoint + b] != 0xffffffff)
                {
                    found = false;
                    searchPoint += b;
                    if (searchPoint == lastSearchPoint)
                        return 0xffffffff;
                }
            }
            if (found)
            {
                for (uint32 b = 0; b < bonesAmount; b++)
                    bufferBonesOccupation[searchPoint + b] = cId;
                cId++;
                lastSearchPoint = searchPoint + bonesAmount;
                return searchPoint;
            }
        }
        else
        {
            // occupied block
            searchPoint++;
            if (searchPoint >= MAX_BONES_IN_SCENE)
                searchPoint = 0;
            if (searchPoint == lastSearchPoint)
                return 0xffffffff;
        }
    }
}

void VulkanObjectBuffers::deallocateBonesOfOjbect(int32 index)
{
    int objectId = bufferBonesOccupation[index];
    if (objectId == 0xffffffff)
        return;
    while (index < MAX_BONES_IN_SCENE && bufferBonesOccupation[index] == objectId)
    {
        bufferBonesOccupation[index] = 0xffffffff;
        index++;
    }
}

void VulkanObjectBuffers::setBoneMatrix(int32 index, const Matrix4x4 &mTransformation)
{
    bufferBonesMapped[currentInFlight][index] = mTransformation;
}

int32 VulkanObjectBuffers::allocateBoneWeightsForObject(uint32 vertexAmount)
{
    static uint32 cId = 0;
    static uint32 lastSearchPoint = 0;
    uint32 searchPoint = lastSearchPoint;

    while (true)
    {
        if (searchPoint + vertexAmount >= MAX_BONES_IN_SCENE)
            searchPoint = 0;
        if (bufferBoneWeightsOccupation[searchPoint] == 0xffffffff)
        {
            // free block;
            bool found = true;
            for (uint32 b = 0; b < vertexAmount; b++)
            {
                if (bufferBoneWeightsOccupation[searchPoint + b] != 0xffffffff)
                {
                    found = false;
                    searchPoint += b;
                    if (searchPoint == lastSearchPoint)
                        return 0xffffffff;
                }
            }
            if (found)
            {
                for (uint32 b = 0; b < vertexAmount; b++)
                    bufferBoneWeightsOccupation[searchPoint + b] = cId;
                cId++;
                lastSearchPoint = searchPoint + vertexAmount;
                return searchPoint;
            }
        }
        else
        {
            // occupied block
            searchPoint++;
            if (searchPoint >= MAX_BONES_IN_SCENE)
                searchPoint = 0;
            if (searchPoint == lastSearchPoint)
                return 0xffffffff;
        }
    }
}

void VulkanObjectBuffers::deallocateBoneWeightsOfOjbect(int32 index)
{
    uint сId = bufferBoneWeightsOccupation[index];
    if (сId == 0xffffffff)
        return;
    while (index < MAX_BONES_IN_SCENE && bufferBoneWeightsOccupation[index] == сId)
    {
        bufferBoneWeightsOccupation[index] = 0xffffffff;
        index++;
    }
}

VulkanObjectBuffers::BoneWeightBinding *VulkanObjectBuffers::getBoneWeightsForObject(int32 index)
{
    return &bufferBoneWeightsMapped[index];
}