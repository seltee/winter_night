#include "features/renderer/vulkan/vulkanInstance.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <vector>
#include <set>
#include <optional>
#include <algorithm>
#include <iostream>

bool _checkDeviceExtensionSupport(VkPhysicalDevice device);

VulkanInstance::~VulkanInstance()
{
    if (vDevice)
    {
        VkDevice device = vDevice->getDevice();
        vkDeviceWaitIdle(device);
        delete vDevice;
    }
    for (const auto &frame : frames)
        delete frame;
    if (surface)
        vkDestroySurfaceKHR(instance, surface, nullptr);
    if (instance)
        vkDestroyInstance(instance, nullptr);
    if (swapChain)
        delete swapChain;
    if (vulkanInstanceExtensions)
        delete vulkanInstanceExtensions;
    if (pipeline)
        delete pipeline;
    if (defaultShader)
        delete defaultShader;
    if (frameBuffer)
        delete frameBuffer;
}

bool VulkanInstance::initNT(void *hWnd, int width, int height)
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

bool VulkanInstance::init(int width, int height, VkSurfaceKHR surface)
{
    this->surface = surface;

    vDevice = new VulkanDevice(instance, surface);
    if (!vDevice->setup())
    {
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

    swapChain = new VulkanSwapChain(device);
    if (!swapChain->setup(width, height, physicalDevice, surface))
    {
        std::cout << "Unable to create swap chain" << std::endl;
        return false;
    }

    renderPass = new VulkanRenderPass();
    if (!renderPass->setup(swapChain->getImageFormat(), device))
    {
        std::cout << "Unable to create render pass" << std::endl;
        return false;
    }

    defaultShader = new VulkanShader();
    if (!defaultShader->makeFromFiles("./shaders/shader.vert.spv", "./shaders/shader.frag.spv", device))
    {
        std::cout << "Unable to compule default shader" << std::endl;
        return false;
    }

    pipeline = new VulkanPipeline();
    if (!pipeline->setup(width, height, swapChain->getExtent(), device, renderPass, defaultShader))
    {
        std::cout << "Unable to create vulkan pipeline" << std::endl;
        return false;
    }

    frameBuffer = new VulkanFrameBuffer(device);
    if (!frameBuffer->setup(swapChain, renderPass))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        return false;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VulkanFrame *frame = new VulkanFrame(vDevice, swapChain);
        if (!frame->setup(renderPass, frameBuffer))
        {
            delete frame;
            std::cout << "Unable to create frame " << i << std::endl;
            return false;
        }
        frames.push_back(frame);
    }

    return true;
}

void VulkanInstance::changeSize(int width, int height)
{
    VkPhysicalDevice physicalDevice = vDevice->getPhysicalDevice();
    VkDevice device = vDevice->getDevice();

    vkDeviceWaitIdle(device);

    if (swapChain)
        delete swapChain;

    swapChain = new VulkanSwapChain(device);
    if (!swapChain->setup(width, height, physicalDevice, surface))
    {
        std::cout << "Unable to create swap chain" << std::endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    if (pipeline)
        delete pipeline;
    pipeline = new VulkanPipeline();
    if (!pipeline->setup(width, height, swapChain->getExtent(), device, renderPass, defaultShader))
    {
        std::cout << "Unable to create vulkan pipeline" << std::endl;
        throw std::runtime_error("failed to recreate pipeline");
    }

    if (frameBuffer)
        delete frameBuffer;

    frameBuffer = new VulkanFrameBuffer(device);
    if (!frameBuffer->setup(swapChain, renderPass))
    {
        std::cout << "Unable to create vulkan frame buffer" << std::endl;
        throw std::runtime_error("failed to recreate swap chain");
    }

    for (const auto &frame : frames)
        delete frame;
    frames.clear();

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VulkanFrame *frame = new VulkanFrame(vDevice, swapChain);
        if (!frame->setup(renderPass, frameBuffer))
        {
            delete frame;
            std::cout << "Unable to create frame " << i << std::endl;
            throw std::runtime_error("failed to recreate frame");
        }
        frames.push_back(frame);
    }
}

void VulkanInstance::render()
{
    frames[currentFrame]->render(pipeline, graphicsQueue, presentQueue);
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
