#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/data/light.h"
#include "core/core.h"
#include <memory>
#include <vector>

namespace wne
{
    class VulkanUtils;
    class VulkanDepthBuffer;
    class VulkanTexture;

    class VulkanObjectBuffers
    {
    public:
        static const uint32 AMOUNT_OF_OBJECTS = 8192;
        static const uint32 AMOUNT_OF_LIGHTS = 128;
        static const uint32 MAX_LIGHT_SHADOWS = 16;
        static const int32 MAX_BONES_IN_SCENE = 4096;
        static const int32 MAX_BONE_WEIGHTS = 5;
        static const int32 MAX_BONE_BINDINGS = (1024 * 256);

        struct GlobalData
        {
            Vector4 ambientLightColor;
            Vector4 cameraPosition;
            uint32 useRadianceMap;
            float radienceMapFactor;
            uint32 pad1, pad2;
        };

        struct LightData
        {
            Vector4 position;
            Vector4 direction;
            Vector4 color;
            float affectRadius, cutOff, outerCutOff, fPad3;
            uint32 shadowId, amountOfCascades;
            float texelSize, pad3;
            uint32 enableDirectional, enableOmni, enableSpot, pad1;
        };

        struct BoneWeightBinding
        {
            int boneIndex[MAX_BONE_WEIGHTS];
            float boneWeight[MAX_BONE_WEIGHTS];
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
            float texelSize,
            const Vector4 &position,
            const Vector4 &direction,
            const Vector4 &color);
        void updateLightShadowData(
            uint32 shadowId,
            Matrix4x4 &mLightMVP);

        bool setup(uint maxFramesInFlight);
        void swap();

        uint32 getNewObjectId();
        void freeObjectId(uint32 objectId);
        uint32 getNewLightId();
        void freeLightId(uint32 lightId);

        void setGlobalData(
            const Vector4 &ambientColor,
            const Vector4 &cameraPosition,
            uint useRadianceMap,
            float radienceMapFactor);

        // returns starting bone index
        int32 allocateBonesForObject(uint32 bonesAmount);
        void deallocateBonesOfOjbect(int32 index);
        void setBoneMatrix(int32 index, const Matrix4x4 &mTransformation);

        int32 allocateBoneWeightsForObject(uint32 vertexAmount);
        void deallocateBoneWeightsOfOjbect(int32 index);
        BoneWeightBinding *getBoneWeightsForObject(int32 index);

        inline uint getFrameInFlight()
        {
            return currentInFlight;
        }

        inline uint getFramesMaxInFlight()
        {
            return maxFramesInFlight;
        }

        VkBuffer getMVPMatricesBuffer(uint frame)
        {
            return bufferMVPMatrices[frame];
        }

        VkBuffer getModelMatricesBuffer(uint frame)
        {
            return bufferModelMatrices[frame];
        }

        VkBuffer getNormalMatricesBuffer(uint frame)
        {
            return bufferNormalMatrices[frame];
        }

        VkBuffer getGlobalDataBuffer(uint frame)
        {
            return bufferGlobalData[frame];
        }

        VkBuffer getLightMVPsBuffer(uint frame)
        {
            return bufferLightMVPsData[frame];
        }

        VkBuffer getLightsDataBuffer(uint frame)
        {
            return bufferLightsData[frame];
        }

        VkBuffer getBonesBuffer(uint frame)
        {
            return bufferBonesData[frame];
        }

        VkBuffer getBoneWeightsBuffer()
        {
            return bufferBoneWeightsData;
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

        constexpr uint64 getBonesBufferSize()
        {
            return sizeof(Matrix4x4) * MAX_BONES_IN_SCENE;
        }

        constexpr uint64 getBoneWeightsBufferSize()
        {
            return sizeof(BoneWeightBinding) * MAX_BONE_BINDINGS;
        }

        constexpr uint64 getLightMVPsBufferSize()
        {
            return sizeof(Matrix4x4) * MAX_LIGHT_SHADOWS;
        }

    protected:
        uint maxFramesInFlight = 0;
        uint currentInFlight = 0;

        VulkanUtils *vulkanUtils = nullptr;

        uint32 searchObjectIndex = 0;
        uint8 bufferObjectsOccupied[AMOUNT_OF_OBJECTS]{};

        uint32 searchLightIndex = 0;
        uint8 bufferLightsOccupied[AMOUNT_OF_LIGHTS]{};

        std::vector<VkBuffer> bufferModelMatrices;
        std::vector<VkDeviceMemory> bufferModelMatricesMemory;
        std::vector<Matrix4x4 *> bufferModelMatricesMapped;

        std::vector<VkBuffer> bufferMVPMatrices;
        std::vector<VkDeviceMemory> bufferMVPMatricesMemory;
        std::vector<Matrix4x4 *> bufferMVPMatricesMapped;

        std::vector<VkBuffer> bufferNormalMatrices;
        std::vector<VkDeviceMemory> bufferNormalMatricesMemory;
        std::vector<Matrix4x4 *> bufferNormalMatricesMapped;

        std::vector<VkBuffer> bufferGlobalData;
        std::vector<VkDeviceMemory> bufferGlobalDataMemory;
        std::vector<GlobalData *> bufferGlobalDataMapped;

        std::vector<VkBuffer> bufferLightMVPsData;
        std::vector<VkDeviceMemory> bufferLightMVPsMemory;
        std::vector<Matrix4x4 *> bufferLightMVPsMapped;

        std::vector<VkBuffer> bufferLightsData;
        std::vector<VkDeviceMemory> bufferLightsDataMemory;
        std::vector<LightData *> bufferLightsDataMapped;

        std::vector<VkBuffer> bufferBonesData;
        std::vector<VkDeviceMemory> bufferBonesMemory;
        std::vector<Matrix4x4 *> bufferBonesMapped;
        uint32 bufferBonesOccupation[MAX_BONES_IN_SCENE]{};

        VkBuffer bufferBoneWeightsData;
        VkDeviceMemory bufferBoneWeightsMemory;
        BoneWeightBinding *bufferBoneWeightsMapped;
        uint32 bufferBoneWeightsOccupation[MAX_BONE_BINDINGS]{};

        std::unique_ptr<VulkanDepthBuffer> dummyBuffer;
    };
};