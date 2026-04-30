#include "features/renderer/vulkan/vulkanInstance.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <vector>
#include <set>
#include <optional>
#include <algorithm>
#include <iostream>

using namespace WNE;

bool _checkDeviceExtensionSupport(VkPhysicalDevice device);

VulkanInstance::~VulkanInstance()
{
    if (vDevice)
    {
        VkDevice device = vDevice->getDevice();
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
    this->surface = surface;

    vDevice = std::make_unique<VulkanDevice>(instance, surface);
    if (!vDevice->setup())
    {
        vDevice.reset();
        std::cout << "Unable to create device" << std::endl;
        return false;
    }
    VkPhysicalDevice physicalDevice = vDevice->getPhysicalDevice();
    VkDevice device = vDevice->getDevice();

    VulkanQueueFamilies vulkanQueueFamilies;
    if (!vulkanQueueFamilies.setup(physicalDevice, surface))
    {
        std::cout << "Unable to setup queue families" << std::endl;
        return false;
    }

    vkGetDeviceQueue(device, vulkanQueueFamilies.getGraphicsFamily().value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, vulkanQueueFamilies.getPresentFamily().value(), 0, &presentQueue);

    swapChain = std::make_unique<VulkanSwapChain>(device);
    if (!swapChain->setup(width, height, physicalDevice, surface))
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
        auto frame = std::make_unique<VulkanFrame>(vDevice.get(), swapChain.get());
        if (!frame->setup(renderPass.get(), frameBuffer.get()))
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
    VkPhysicalDevice physicalDevice = vDevice->getPhysicalDevice();
    VkDevice device = vDevice->getDevice();

    vkDeviceWaitIdle(device);

    swapChain = std::make_unique<VulkanSwapChain>(device);
    if (!swapChain->setup(width, height, physicalDevice, surface))
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
        auto frame = std::make_unique<VulkanFrame>(vDevice.get(), swapChain.get());
        if (!frame->setup(renderPass.get(), frameBuffer.get()))
        {
            std::cout << "Unable to create frame " << i << std::endl;
            throw std::runtime_error("failed to recreate frame");
        }
        frames.emplace_back(std::move(frame));
    }
}

void VulkanInstance::render()
{
    frames[currentFrame]->render(pipeline.get(), graphicsQueue, presentQueue);
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
