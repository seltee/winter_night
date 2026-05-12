#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanUtils;

    class VulkanObjectBuffers
    {
    public:
        static const uint32 AMOUNT_OF_OBJECTS = 256;

        struct GlobalData
        {
            Vector4 ambientLightColor;
            Vector4 u1;
            Vector4 u2;
            Vector4 u3;
        };

        VulkanObjectBuffers(VulkanUtils *vulkanUtils);
        ~VulkanObjectBuffers();

        void updateObjectData(uint32 objectId, const Matrix4x4 &mModel, const Matrix4x4 &mNormal, const Matrix4x4 &mMVP) noexcept;

        bool setup();

        uint32 getNewObjectId();
        void freeObjectId(uint32 objectId);

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

        constexpr uint64 getBufferSize()
        {
            return sizeof(Matrix4x4) * AMOUNT_OF_OBJECTS;
        }

        constexpr uint64 getGlobalDataSize()
        {
            return sizeof(GlobalData);
        }

    protected:
        VulkanUtils *vulkanUtils = nullptr;

        uint32 searchIndex = 0;
        uint8 bufferOccupied[AMOUNT_OF_OBJECTS];

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
    };
};