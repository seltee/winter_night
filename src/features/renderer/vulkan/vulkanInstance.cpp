#include "features/renderer/vulkan/vulkanInstance.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <vector>
#include <set>
#include <optional>
#include <algorithm>
#include <iostream>

using namespace wne;

bool _checkDeviceExtensionSupport(VkPhysicalDevice device);

VulkanInstance::~VulkanInstance()
{
    if (vulkanDevice)
    {
        VkDevice device = vulkanDevice->getDevice();
        vkDeviceWaitIdle(device);
    }
    if (surface)
        vkDestroySurfaceKHR(instance, surface, nullptr);
    if (instance)
        vkDestroyInstance(instance, nullptr);
    if (vulkanInstanceExtensions)
        delete vulkanInstanceExtensions;
}

bool VulkanInstance::initNT(void *hWnd, uint32 width, uint32 height)
{
    if (!initInstance())
    {
        std::cout << "Unable to init instance" << std::endl;
        return false;
    }

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = (HWND__ *)hWnd;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    VkSurfaceKHR surface;
    VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        std::cout << "failed to create window surface" << std::endl;
        std::cout << "%i" << result << std::endl;
        return false;
    }

    return init(width, height, surface);
}

bool VulkanInstance::init(uint32 width, uint32 height, VkSurfaceKHR surface)
{
    // get rid of throw
    this->width = width;
    this->height = height;
    this->surface = surface;

    vulkanDevice = std::make_unique<VulkanDevice>(instance, surface);
    if (!vulkanDevice->setup())
    {
        vulkanDevice.reset();
        std::cout << "Unable to create device" << std::endl;
        return false;
    }
    VkPhysicalDevice physicalDevice = vulkanDevice->getPhysicalDevice();
    VkDevice device = vulkanDevice->getDevice();

    VulkanQueueFamilies vulkanQueueFamilies;
    if (!vulkanQueueFamilies.setup(physicalDevice, surface))
    {
        std::cout << "Unable to setup queue families" << std::endl;
        return false;
    }

    vkGetDeviceQueue(device, vulkanQueueFamilies.getGraphicsFamily().value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, vulkanQueueFamilies.getPresentFamily().value(), 0, &presentQueue);

    commandPool = std::make_unique<VulkanCommandPool>(device, physicalDevice);
    if (!commandPool->setup(surface))
    {
        std::cout << "Unable to create command pool" << std::endl;
        throw std::runtime_error("failed to create command pool");
    }
    
    utils = std::make_unique<VulkanUtils>(vulkanDevice.get(), commandPool.get(), graphicsQueue, presentQueue);

    swapChain = std::make_unique<VulkanSwapChain>(device);
    if (!swapChain->setup(width, height, physicalDevice, surface, !isImmidiateSwap))
    {
        std::cout << "Unable to create swap chain" << std::endl;
        return false;
    }

    renderPass = std::make_unique<VulkanRenderPass>();
    if (!renderPass->setup(swapChain->getImageFormat(), device))
    {
        std::cout << "Unable to create render pass" << std::endl;
        return false;
    }

    defaultShader = std::make_unique<VulkanShader>();
    if (!defaultShader->makeFromFiles("./shaders/shader.vert.spv", "./shaders/shader.frag.spv", device))
    {
        std::cout << "Unable to compule default shader" << std::endl;
        return false;
    }

    pipeline = std::make_unique<VulkanPipeline>();
    if (!pipeline->setup(width, height, swapChain->getExtent(), device, renderPass.get(), defaultShader.get()))
    {
        std::cout << "Unable to create vulkan pipeline" << std::endl;
        return false;
    }

    frameBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameBuffer->setup(swapChain.get(), renderPass.get()))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        return false;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto frame = std::make_unique<VulkanFrame>(vulkanDevice.get(), swapChain.get());
        if (!frame->setup(renderPass.get(), frameBuffer.get(), commandPool.get()))
        {
            std::cout << "Unable to create frame " << i << std::endl;
            return false;
        }
        frames.emplace_back(std::move(frame));
    }

    return true;
}

void VulkanInstance::changeSize(uint32 width, uint32 height)
{
    this->width = width;
    this->height = height;

    VkPhysicalDevice physicalDevice = vulkanDevice->getPhysicalDevice();
    VkDevice device = vulkanDevice->getDevice();

    vkDeviceWaitIdle(device);

    swapChain = std::make_unique<VulkanSwapChain>(device);
    if (!swapChain->setup(width, height, physicalDevice, surface, !isImmidiateSwap))
    {
        std::cout << "Unable to create swap chain" << std::endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    pipeline = std::make_unique<VulkanPipeline>();
    if (!pipeline->setup(width, height, swapChain->getExtent(), device, renderPass.get(), defaultShader.get()))
    {
        std::cout << "Unable to create vulkan pipeline" << std::endl;
        throw std::runtime_error("failed to recreate pipeline");
    }

    frameBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameBuffer->setup(swapChain.get(), renderPass.get()))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    frames.clear();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto frame = std::make_unique<VulkanFrame>(vulkanDevice.get(), swapChain.get());
        if (!frame->setup(renderPass.get(), frameBuffer.get(), commandPool.get()))
        {
            std::cout << "Unable to create frame " << i << std::endl;
            throw std::runtime_error("failed to recreate frame");
        }
        frames.emplace_back(std::move(frame));
    }
}

void VulkanInstance::setSyncState(bool syncEnabled)
{
    isImmidiateSwap = syncEnabled;
    changeSize(width, height);
}

bool VulkanInstance::getSyncState()
{
    return isImmidiateSwap;
}

void VulkanInstance::startRendering()
{
    frames[currentFrame]->startFrame(pipeline.get());
}

void VulkanInstance::finishRendering()
{
    frames[currentFrame]->finishFrame(graphicsQueue, presentQueue);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

bool VulkanInstance::initInstance()
{
    vulkanInstanceExtensions = new VulkanInstanceExtensions();
    if (!vulkanInstanceExtensions->hasExtension("VK_KHR_surface") || !vulkanInstanceExtensions->hasExtension("VK_KHR_win32_surface"))
    {
        std::cout << "Required extensions missing" << std::endl;
        return false;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Instance";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = VULKAN_INSTANCE_REQUIRED_EXTENSIONS;
    createInfo.ppEnabledExtensionNames = instanceExtNames;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::cout << "Unable to create Vulkan instance" << std::endl;
        return false;
    }
    return true;
}
