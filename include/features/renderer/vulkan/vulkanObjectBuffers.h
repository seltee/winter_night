#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/data/light.h"
#include "core/core.h"
#include <memory>

namespace wne
{
    class VulkanUtils;
    class VulkanDepthBuffer;

    class VulkanObjectBuffers
    {
    public:
        static const uint32 AMOUNT_OF_OBJECTS = 256;
        static const uint32 AMOUNT_OF_LIGHTS = 128;
        static const uint32 MAX_LIGHT_SHADOWS = 16;

        struct GlobalData
        {
            Vector4 ambientLightColor;
            Vector4 u1;
            Vector4 u2;
            Vector4 u3;
        };

        struct LightData
        {
            Vector4 position;
            Vector4 direction;
            Vector4 color;
            float affectRadius, cutOff, outerCutOff, fPad3;
            uint32 shadowId, amountOfCascades, pad2, pad3;
            uint32 enableDirectional, enableOmni, enableSpot, pad1;
        };

        VulkanObjectBuffers(VulkanUtils *vulkanUtils);
        ~VulkanObjectBuffers();

        void updateObjectData(
            uint32 objectId,
            const Matrix4x4 &mModel,
            const Matrix4x4 &mNormal,
            const Matrix4x4 &mMVP) noexcept;

        void updateLightData(
            uint32 lightId,
            Light::Type type,
            float affectRadius,
            float cutOff,
            float outerCutOff,
            uint32 shadowId,
            uint32 amountOfCascades,
            const Vector4 &position,
            const Vector4 &direction,
            const Vector4 &color);
        void updateLightShadowData(
            uint32 shadowId,
            Matrix4x4 &mLightMVP);

        bool setup();

        uint32 getNewObjectId();
        void freeObjectId(uint32 objectId);
        uint32 getNewLightId();
        void freeLightId(uint32 lightId);

        void setAmbientColor(Vector4 &ambientColor);

        VkBuffer getModelMatricesBuffer()
        {
            return bufferModelMatrices;
        }

        VkBuffer getMVPMatricesBuffer()
        {
            return bufferMVPMatrices;
        }

        VkBuffer getNormalMatricesBuffer()
        {
            return bufferNormalMatrices;
        }

        VkBuffer getGlobalDataBuffer()
        {
            return bufferGlobalData;
        }

        VkBuffer getLightMVPsBuffer()
        {
            return bufferLightMVPsData;
        }

        VkBuffer getLightsDataBuffer()
        {
            return bufferLightsData;
        }

        inline VulkanDepthBuffer *getDummyDepthBuffer()
        {
            return dummyBuffer.get();
        }

        constexpr uint64 getMatrixBufferSize()
        {
            return sizeof(Matrix4x4) * AMOUNT_OF_OBJECTS;
        }

        constexpr uint64 getGlobalDataSize()
        {
            return sizeof(GlobalData);
        }

        constexpr uint64 getLightsBufferSize()
        {
            return sizeof(LightData) * AMOUNT_OF_LIGHTS;
        }

        constexpr uint64 getLightMVPsBufferSize()
        {
            return sizeof(Matrix4x4) * MAX_LIGHT_SHADOWS;
        }

    protected:
        VulkanUtils *vulkanUtils = nullptr;

        uint32 searchObjectIndex = 0;
        uint8 bufferObjectsOccupied[AMOUNT_OF_OBJECTS]{};

        uint32 searchLightIndex = 0;
        uint8 bufferLightsOccupied[AMOUNT_OF_LIGHTS]{};

        VkBuffer bufferModelMatrices = nullptr;
        VkDeviceMemory bufferModelMatricesMemory = nullptr;
        Matrix4x4 *bufferModelMatricesMapped = nullptr;

        VkBuffer bufferMVPMatrices = nullptr;
        VkDeviceMemory bufferMVPMatricesMemory = nullptr;
        Matrix4x4 *bufferMVPMatricesMapped = nullptr;

        VkBuffer bufferNormalMatrices = nullptr;
        VkDeviceMemory bufferNormalMatricesMemory = nullptr;
        Matrix4x4 *bufferNormalMatricesMapped = nullptr;

        VkBuffer bufferGlobalData = nullptr;
        VkDeviceMemory bufferGlobalDataMemory = nullptr;
        GlobalData *bufferGlobalDataMapped = nullptr;

        VkBuffer bufferLightMVPsData = nullptr;
        VkDeviceMemory bufferLightMVPsMemory = nullptr;
        Matrix4x4 *bufferLightMVPsMapped = nullptr;

        VkBuffer bufferLightsData = nullptr;
        VkDeviceMemory bufferLightsDataMemory = nullptr;
        LightData *bufferLightsDataMapped = nullptr;

        std::unique_ptr<VulkanDepthBuffer> dummyBuffer;
    };
};