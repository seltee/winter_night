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

std::unique_ptr<VulkanInstance> VulkanInstance::create(void *hwnd)
{
    auto instance = std::unique_ptr<VulkanInstance>(new VulkanInstance());
    if (!instance->initNT(hwnd))
        return nullptr;
    return instance;
}

bool VulkanInstance::initNT(void *hWnd)
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

    return init(surface);
}

bool VulkanInstance::init(VkSurfaceKHR surface)
{
    // get rid of throw
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

    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
    width = caps.currentExtent.width;
    height = caps.currentExtent.height;

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
        std::cout << "failed to create command pool" << std::endl;
        return false;
    }

    vulkanUtils = std::make_unique<VulkanUtils>(
        vulkanDevice.get(),
        commandPool.get(),
        graphicsQueue,
        presentQueue);
    if (!vulkanUtils->setup())
    {
        std::cout << "Unable to create utils" << std::endl;
        return false;
    }

    swapChain = std::make_unique<VulkanSwapChain>(vulkanUtils.get());
    if (!swapChain->setup(width, height, surface, !isImmidiateSwap, MSAASampleCount))
    {
        std::cout << "Unable to create swap chain" << std::endl;
        return false;
    }

    renderPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!renderPass->setupColor(swapChain->getImageFormat(), vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        std::cout << "Unable to create render pass" << std::endl;
        return false;
    }

    depthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!depthPass->setupDepth(true, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        std::cout << "Unable to create depth pass" << std::endl;
        return false;
    }

    shadowDepthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!shadowDepthPass->setupDepth(true, 1))
    {
        std::cout << "Unable to create depth pass" << std::endl;
        return false;
    }
    vulkanUtils->rebuildPipelines(swapChain.get(), renderPass.get(), depthPass.get(), shadowDepthPass.get(), MSAASampleCount);

    depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils.get());
    if (!depthBuffer->setup(width, height, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount), true))
    {
        std::cout << "unable to create depth buffer" << std::endl;
        return false;
    }

    frameColorBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameColorBuffer->setupColor(swapChain.get(), renderPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        return false;
    }

    frameDepthBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameDepthBuffer->setupDepth(swapChain.get(), depthPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        return false;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto frame = std::make_unique<VulkanFrame>(vulkanDevice.get(), swapChain.get());
        if (!frame->setup(commandPool.get(), vulkanUtils.get()))
        {
            std::cout << "Unable to create frame " << i << std::endl;
            return false;
        }
        frames.emplace_back(std::move(frame));
    }

    return true;
}

void VulkanInstance::changeSize()
{
    VkDevice device = vulkanDevice->getDevice();

    vkDeviceWaitIdle(device);
    frames.clear();
    frameColorBuffer.reset();
    frameDepthBuffer.reset();
    vulkanUtils->destroyPipelines();
    renderPass.reset();
    depthPass.reset();
    shadowDepthPass.reset();
    depthBuffer.reset();
    swapChain.reset();
    vkDeviceWaitIdle(device);

    VkSurfaceCapabilitiesKHR caps{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanDevice->getPhysicalDevice(), surface, &caps);
    width = caps.currentExtent.width;
    height = caps.currentExtent.height;

    swapChain = std::make_unique<VulkanSwapChain>(vulkanUtils.get());
    if (!swapChain->setup(width, height, surface, !isImmidiateSwap, MSAASampleCount))
    {
        std::cout << "Unable to create swap chain" << std::endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    renderPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!renderPass->setupColor(swapChain->getImageFormat(), vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        std::cout << "Unable to create render pass" << std::endl;
        throw std::runtime_error("failed to recreate render pass");
    }

    depthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!depthPass->setupDepth(true, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        std::cout << "Unable to create depth pass" << std::endl;
        throw std::runtime_error("failed to recreate depth pass");
    }

    shadowDepthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!shadowDepthPass->setupDepth(true, 1))
    {
        std::cout << "Unable to create shadow depth pass" << std::endl;
        throw std::runtime_error("failed to recreate shadow depth pass");
    }

    vulkanUtils->rebuildPipelines(swapChain.get(), renderPass.get(), depthPass.get(), shadowDepthPass.get(), MSAASampleCount);

    depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils.get());
    if (!depthBuffer->setup(width, height, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount), true))
    {
        std::cout << "unable to create depth buffer" << std::endl;
        throw std::runtime_error("unable to create depth buffer");
    }

    frameColorBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameColorBuffer->setupColor(swapChain.get(), renderPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    frameDepthBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameDepthBuffer->setupDepth(swapChain.get(), depthPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        throw std::runtime_error("Unable to create vulkan frame buffer");
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto frame = std::make_unique<VulkanFrame>(vulkanDevice.get(), swapChain.get());
        if (!frame->setup(commandPool.get(), vulkanUtils.get()))
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
    changeSize();
}

bool VulkanInstance::getSyncState()
{
    return isImmidiateSwap;
}

void VulkanInstance::startRendering()
{
    uint currentFrame = vulkanUtils->getCurrentFrame();
    frames[currentFrame]->startFrame();
    vulkanUtils->setCurrentCommandBuffer(frames[currentFrame]->getCommandBuffer());
}

void VulkanInstance::beginDepthPass()
{
    uint currentFrame = vulkanUtils->getCurrentFrame();
    frames[currentFrame]->beginDepthPass(depthPass.get(), frameDepthBuffer.get());
}

void VulkanInstance::finishDepthPass()
{
    uint currentFrame = vulkanUtils->getCurrentFrame();
    frames[currentFrame]->endDepthPass();
}

void VulkanInstance::beginRenderPass()
{
    uint currentFrame = vulkanUtils->getCurrentFrame();
    frames[currentFrame]->beginRenderPass(renderPass.get(), frameColorBuffer.get());
}

void VulkanInstance::finishRendering()
{
    uint currentFrame = vulkanUtils->getCurrentFrame();
    frames[currentFrame]->finishFrame(graphicsQueue, presentQueue);
}

void VulkanInstance::waitIdle()
{
    vkQueueWaitIdle(vulkanUtils->getPresentQueue());
}

void VulkanInstance::updateMSAASampleCount(uint MSAASampleCount)
{
    this->MSAASampleCount = MSAASampleCount;
    changeSize();
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

    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames =
        validationLayers.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::cout << "Unable to create Vulkan instance" << std::endl;
        return false;
    }
    return true;
}
