#include "features/renderer/vulkan/vulkanInstance.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <vector>
#include <set>
#include <optional>
#include <algorithm>

using namespace wne;

bool _checkDeviceExtensionSupport(VkPhysicalDevice device);

VulkanInstance::~VulkanInstance()
{
    if (vulkanDevice)
    {
        VkDevice device = vulkanDevice->getDevice();
        vkDeviceWaitIdle(device);
    }
    if (vulkanSurface)
        vkDestroySurfaceKHR(instance, vulkanSurface, nullptr);
    if (instance)
        vkDestroyInstance(instance, nullptr);
    if (vulkanInstanceExtensions)
        delete vulkanInstanceExtensions;
}

std::unique_ptr<VulkanInstance> VulkanInstance::createNT(void *hwnd, int32 width, int32 height)
{
    auto instance = std::unique_ptr<VulkanInstance>(new VulkanInstance());
    if (!instance->initNT(hwnd, width, height))
        return nullptr;
    return instance;
}

std::unique_ptr<VulkanInstance> VulkanInstance::createLinuxWayland(void *wlDisplay, void *wlSurface, int32 width, int32 height)
{
    auto instance = std::unique_ptr<VulkanInstance>(new VulkanInstance());
    if (!instance->initLinuxWayland(wlDisplay, wlSurface, width, height))
        return nullptr;
    return instance;
}

bool VulkanInstance::initNT(void *hWnd, int32 width, int32 height)
{
#if defined(OS_WINDOWS)
    if (!initInstance())
    {
        Logger::log << "Unable to init instance" << endl;
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
        Logger::log << "failed to create window surface " << result << endl;
        return false;
    }

    return init(surface, width, height);
#else
    return false;
#endif
}

bool VulkanInstance::initLinuxWayland(void *wlDisplay, void *wlSurface, int32 width, int32 height)
{
#if defined(OS_LINUX)
    if (!initInstance())
    {
        Logger::log << "Unable to init instance" << endl;
        return false;
    }

    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.display = (wl_display *)wlDisplay; // struct wl_display*
    surfaceCreateInfo.surface = (wl_surface *)wlSurface; // struct wl_surface*

    VkSurfaceKHR surface;
    VkResult result = vkCreateWaylandSurfaceKHR(
        instance,
        &surfaceCreateInfo,
        nullptr,
        &surface);
    if (result != VK_SUCCESS)
    {
        Logger::log << "Unable to create Vulkan surface for Wayland" << endl;
        return false;
    }

    return init(surface, width, height);
#else
    return false;
#endif
}

bool VulkanInstance::init(VkSurfaceKHR vulkanSurface, int32 width, int32 height)
{
    // get rid of throw
    this->vulkanSurface = vulkanSurface;
    if (!vulkanSurface)
    {
        Logger::log << "Vulkan surface wasn't provided" << endl;
        return false;
    }

    vulkanDevice = std::make_unique<VulkanDevice>(instance, vulkanSurface);
    if (!vulkanDevice->setup())
    {
        vulkanDevice.reset();
        Logger::log << "Unable to create device" << endl;
        return false;
    }
    VkPhysicalDevice physicalDevice = vulkanDevice->getPhysicalDevice();
    VkDevice device = vulkanDevice->getDevice();

    VkSurfaceCapabilitiesKHR caps;
    VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vulkanSurface, &caps);
    if (res != VK_SUCCESS)
    {
        Logger::log << "Unable to get surface data" << endl;
        return false;
    }
    if (width < (int)caps.minImageExtent.width || width > (int)caps.maxImageExtent.width)
        width = (int)caps.currentExtent.width;
    if (height < (int)caps.minImageExtent.height || height > (int)caps.maxImageExtent.height)
        height = (int)caps.currentExtent.height;

    this->width = width;
    this->height = height;

    VulkanQueueFamilies vulkanQueueFamilies;
    if (!vulkanQueueFamilies.setup(physicalDevice, vulkanSurface))
    {
        Logger::log << "Unable to setup queue families" << endl;
        return false;
    }

    vkGetDeviceQueue(device, vulkanQueueFamilies.getGraphicsFamily().value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, vulkanQueueFamilies.getPresentFamily().value(), 0, &presentQueue);

    VkBool32 supported = VK_FALSE;
    VkResult result;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(
        physicalDevice,
        vulkanQueueFamilies.getPresentFamily().value(),
        vulkanSurface,
        &supported);
    if (result != VK_SUCCESS)
    {
        Logger::log << "Created surface is not supported for rendering" << endl;
        return false;
    }

    commandPool = std::make_unique<VulkanCommandPool>(device, physicalDevice);
    if (!commandPool->setup(vulkanSurface))
    {
        Logger::log << "failed to create command pool" << endl;
        return false;
    }

    vulkanUtils = std::make_unique<VulkanUtils>(
        vulkanDevice.get(),
        commandPool.get(),
        graphicsQueue,
        presentQueue);
    if (!vulkanUtils->setup())
    {
        Logger::log << "Unable to create utils" << endl;
        return false;
    }
    vulkanUtils->logSystemData();

    swapChain = std::make_unique<VulkanSwapChain>(vulkanUtils.get());
    if (!swapChain->setup(width, height, vulkanSurface, !isImmidiateSwap, MSAASampleCount))
    {
        Logger::log << "Unable to create swap chain" << endl;
        return false;
    }

    renderPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!renderPass->setupColor(swapChain->getImageFormat(), vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        Logger::log << "Unable to create render pass" << endl;
        return false;
    }

    depthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!depthPass->setupDepth(true, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        Logger::log << "Unable to create depth pass" << endl;
        return false;
    }

    shadowDepthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!shadowDepthPass->setupDepth(true, 1))
    {
        Logger::log << "Unable to create depth pass" << endl;
        return false;
    }
    vulkanUtils->rebuildPipelines(swapChain.get(), renderPass.get(), depthPass.get(), shadowDepthPass.get(), MSAASampleCount);

    depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils.get());
    if (!depthBuffer->setup(width, height, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount), true))
    {
        Logger::log << "unable to create depth buffer" << endl;
        return false;
    }

    frameColorBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameColorBuffer->setupColor(swapChain.get(), renderPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        Logger::log << "Unable to create vulkan frame buffer" << endl;
        return false;
    }

    frameDepthBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameDepthBuffer->setupDepth(swapChain.get(), depthPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        Logger::log << "Unable to create vulkan frame buffer" << endl;
        return false;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto frame = std::make_unique<VulkanFrame>(vulkanDevice.get(), swapChain.get());
        if (!frame->setup(commandPool.get(), vulkanUtils.get()))
        {
            Logger::log << "Unable to create frame " << i << endl;
            return false;
        }
        frames.emplace_back(std::move(frame));
    }

    return true;
}

void VulkanInstance::changeSize(int32 width, int32 height)
{
    VkDevice device = vulkanDevice->getDevice();

    vkDeviceWaitIdle(device);
    frames.clear();
    frameColorBuffer.reset();
    frameDepthBuffer.reset();
    // vulkanUtils->destroyPipelines();
    renderPass.reset();
    depthPass.reset();
    shadowDepthPass.reset();
    depthBuffer.reset();
    swapChain.reset();
    vkDeviceWaitIdle(device);

    VkSurfaceCapabilitiesKHR caps;
    VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanUtils->getVulkanDevice()->getPhysicalDevice(), vulkanSurface, &caps);
    if (res == VK_SUCCESS)
    {
        if (width < (int)caps.minImageExtent.width || width > (int)caps.maxImageExtent.width)
            width = (int)caps.currentExtent.width;
        if (height < (int)caps.minImageExtent.height || height > (int)caps.maxImageExtent.height)
            height = (int)caps.currentExtent.height;
    }
    this->width = width;
    this->height = height;

    swapChain = std::make_unique<VulkanSwapChain>(vulkanUtils.get());
    if (!swapChain->setup(width, height, vulkanSurface, !isImmidiateSwap, MSAASampleCount))
    {
        Logger::log << "Unable to create swap chain" << endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    renderPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!renderPass->setupColor(swapChain->getImageFormat(), vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        Logger::log << "Unable to create render pass" << endl;
        throw std::runtime_error("failed to recreate render pass");
    }

    depthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!depthPass->setupDepth(true, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount)))
    {
        Logger::log << "Unable to create depth pass" << endl;
        throw std::runtime_error("failed to recreate depth pass");
    }

    shadowDepthPass = std::make_unique<VulkanRenderPass>(vulkanUtils.get());
    if (!shadowDepthPass->setupDepth(true, 1))
    {
        Logger::log << "Unable to create shadow depth pass" << endl;
        throw std::runtime_error("failed to recreate shadow depth pass");
    }

    vulkanUtils->rebuildPipelines(swapChain.get(), renderPass.get(), depthPass.get(), shadowDepthPass.get(), MSAASampleCount);

    depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils.get());
    if (!depthBuffer->setup(width, height, vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount), true))
    {
        Logger::log << "unable to create depth buffer" << endl;
        throw std::runtime_error("unable to create depth buffer");
    }

    frameColorBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameColorBuffer->setupColor(swapChain.get(), renderPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        Logger::log << "Unable to create vulkan frame buffer" << endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    frameDepthBuffer = std::make_unique<VulkanFrameBuffer>(device);
    if (!frameDepthBuffer->setupDepth(swapChain.get(), depthPass.get(), depthBuffer.get(), MSAASampleCount))
    {
        Logger::log << "Unable to create vulkan frame buffer" << endl;
        throw std::runtime_error("Unable to create vulkan frame buffer");
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto frame = std::make_unique<VulkanFrame>(vulkanDevice.get(), swapChain.get());
        if (!frame->setup(commandPool.get(), vulkanUtils.get()))
        {
            Logger::log << "Unable to create frame " << i << endl;
            throw std::runtime_error("failed to recreate frame");
        }
        frames.emplace_back(std::move(frame));
    }

    VulkanMaterial::resetPipelines();
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

    vulkanUtils->incrementCurrentFrameNumber();
    vulkanUtils->processPostponnedRemoval();
}

void VulkanInstance::waitIdle()
{
    vkQueueWaitIdle(vulkanUtils->getPresentQueue());
}

void VulkanInstance::updateMSAASampleCount(uint MSAASampleCount)
{
    this->MSAASampleCount = MSAASampleCount;
    changeSize(width, height);
}

bool VulkanInstance::initInstance()
{
    vulkanInstanceExtensions = new VulkanInstanceExtensions();
    if (!vulkanInstanceExtensions->hasExtension("VK_KHR_surface") || !vulkanInstanceExtensions->hasExtension(getOSSurfaceExtensionName()))
    {
        Logger::log << "Required extensions missing" << endl;
        vulkanInstanceExtensions->dumpExtensions();
        return false;
    }

    const char *const instanceExtNames[VULKAN_INSTANCE_REQUIRED_EXTENSIONS] = {
        "VK_KHR_surface",
        getOSSurfaceExtensionName()};

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

    // validation layers
    VulkanLayers vulkanLayers;
    std::vector<const char *> validationLayers;
    if (vulkanLayers.hasLayer("VK_LAYER_KHRONOS_validation"))
        validationLayers.push_back("VK_LAYER_KHRONOS_validation");

    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames =
        validationLayers.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        Logger::log << "Unable to create Vulkan instance, error " << result << endl;
        return false;
    }
    return true;
}

const char *VulkanInstance::getOSSurfaceExtensionName()
{
#if defined(OS_WINDOWS)
    return "VK_KHR_win32_surface";
#else
    return "VK_KHR_wayland_surface";
#endif
}
