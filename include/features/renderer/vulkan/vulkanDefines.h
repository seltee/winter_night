#pragma once
#include "core/core.h"
#include "core/math.h"
#include "core/platform.h"

#if defined(OS_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(OS_LINUX)
    #define VK_USE_PLATFORM_WAYLAND_KHR
    #define VK_USE_PLATFORM_XCB_KHR
#endif

typedef struct VkInstance_T *VkInstance;
typedef struct VkPhysicalDevice_T *VkPhysicalDevice;
typedef struct VkDevice_T *VkDevice;
typedef struct VkSurfaceKHR_T *VkSurfaceKHR;
typedef struct VkQueue_T *VkQueue;
typedef struct VkSwapchainKHR_T *VkSwapchainKHR;
typedef struct VkPipelineLayout_T *VkPipelineLayout;
typedef struct VkPipeline_T *VkPipeline;
typedef struct VkRenderPass_T *VkRenderPass;
typedef struct VkImage_T *VkImage;
typedef struct VkImageView_T *VkImageView;
typedef struct VkFramebuffer_T *VkFramebuffer;
typedef struct VkCommandPool_T *VkCommandPool;
typedef struct VkCommandBuffer_T *VkCommandBuffer;
typedef struct VkSemaphore_T *VkSemaphore;
typedef struct VkBuffer_T *VkBuffer;
typedef struct VkDeviceMemory_T *VkDeviceMemory;
typedef struct VkFence_T *VkFence;
typedef struct VkDescriptorSetLayout_T *VkDescriptorSetLayout;
typedef struct VkDescriptorPool_T *VkDescriptorPool;
typedef struct VkDescriptorSet_T *VkDescriptorSet;
typedef struct VkSampler_T *VkSampler;
typedef struct VkExtent2D VkExtent2D;
typedef struct VkShaderModule_T *VkShaderModule;
typedef struct VkPipelineShaderStageCreateInfo VkPipelineShaderStageCreateInfo;

using VulkanFormat = uint64;
using VulkanImageTiling = uint64;
using VulkanFormatFeatureFlags = uint64;
using VulkanImageUsageFlags = uint64;
using VulkanMemoryPropertyFlagBits = uint64;
using VulkanImageAspectFlags = uint64;

#define MAX_FRAMES_IN_FLIGHT 2
