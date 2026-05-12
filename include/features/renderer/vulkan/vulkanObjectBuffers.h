#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanUtils;

    class VulkanObjectBuffers
    {
    public:
        static const uint32 AMOUNT_OF_OBJECTS = 256;

        VulkanObjectBuffers(VulkanUtils *vulkanUtils);
        ~VulkanObjectBuffers();

        void updateObjectData(uint32 objectId, const Matrix4x4 &mModel, const Matrix4x4 &mNormal, const Matrix4x4 &mMVP) noexcept;

        bool setup();

        uint32 getNewObjectId();
        void freeObjectId(uint32 objectId);

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

        constexpr uint64 getBufferSize()
        {
            return sizeof(Matrix4x4) * AMOUNT_OF_OBJECTS;
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
    };
};