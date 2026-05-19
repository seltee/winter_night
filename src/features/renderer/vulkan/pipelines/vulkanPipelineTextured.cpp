#include "features/renderer/vulkan/pipelines/vulkanPipelineTextured.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "core/data.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <vector>
#include <iostream>
#include <array>

using namespace wne;

VulkanPipelineTextured::VulkanPipelineTextured(VulkanDevice *vulkanDevice) : VulkanPipeline(vulkanDevice)
{
}

VulkanPipelineTextured::~VulkanPipelineTextured()
{
    auto device = vulkanDevice->getDevice();
    if (graphicsPipeline)
    {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        graphicsPipeline = nullptr;
    }
    if (pipelineLayout)
    {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        pipelineLayout = nullptr;
    }
}

VkPipeline VulkanPipelineTextured::getGraphicsPipeline()
{
    return graphicsPipeline;
}

VkPipelineLayout VulkanPipelineTextured::getPipelineLayout()
{
    return pipelineLayout;
}

bool VulkanPipelineTextured::setupColor(VulkanRenderPass *renderPass)
{
    if (!buildShaderColor())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedColor())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    if (!buildPipeline(2, true, false, true, true, false, renderPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupColorNoLights(VulkanRenderPass *renderPass)
{
    if (!buildShaderColorNoLights())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedColor())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    if (!buildPipeline(2, true, false, true, true, false, renderPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupDepth(VulkanRenderPass *depthPass)
{
    if (!buildShaderDepth())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        std::cout << "Unable to setup textured depth pipeline" << std::endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(1, false, true, true, false, false, depthPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupMaskedDepth(VulkanRenderPass *depthPass)
{
    if (!buildShaderMaskedDepth())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        std::cout << "Unable to setup textured depth pipeline" << std::endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(2, false, true, true, true, false, depthPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupDepthShadow(VulkanRenderPass *depthPass)
{
    if (!buildShaderDepth())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        std::cout << "Unable to setup textured depth pipeline" << std::endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(1, false, true, true, false, false, depthPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupMaskedDepthShadow(VulkanRenderPass *depthPass)
{
    if (!buildShaderMaskedDepth())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        std::cout << "Unable to setup textured depth pipeline" << std::endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(2, false, true, true, true, false, depthPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupAtmosphere(VulkanRenderPass *renderPass)
{
    if (!buildShaderAtmosphere())
    {
        std::cout << "Unable to build shader" << std::endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedColor())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        std::cout << "Unable to setup textured color pipeline" << std::endl;
        return false;
    }

    if (!buildPipeline(2, true, false, false, true, true, renderPass))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

VulkanDescriptorSetLayout *VulkanPipelineTextured::getDescriptorSetLayoutPipeline()
{
    return descriptorSetLayoutPipeline.get();
}

VulkanDescriptorSetLayout *VulkanPipelineTextured::getDescriptorSetLayoutSampler()
{
    return descriptorSetLayoutSampler.get();
}

bool VulkanPipelineTextured::buildShaderColor()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTextured.vert.spv", "./shaders/shaderTextured.frag.spv", device))
    {
        std::cout << "Unable to compile textured shader" << std::endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderColorNoLights()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTexturedNoLight.vert.spv", "./shaders/shaderTexturedNoLight.frag.spv", device))
    {
        std::cout << "Unable to compile textured shader" << std::endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderDepth()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTexturedDepth.vert.spv", "./shaders/shaderTexturedDepth.frag.spv", device))
    {
        std::cout << "Unable to compile textured shader" << std::endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderMaskedDepth()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTexturedDepth.vert.spv", "./shaders/shaderTexturedMaskedDepth.frag.spv", device))
    {
        std::cout << "Unable to compile textured shader" << std::endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderAtmosphere()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderAtmosphere.vert.spv", "./shaders/shaderAtmosphere.frag.spv", device))
    {
        std::cout << "Unable to compile textured shader" << std::endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildPipeline(
    uint32 stageAmount,
    bool enableColorBlending,
    bool enableDepthWrite,
    bool enableDepthTest,
    bool enableSampler,
    bool reverseFaceCooling,
    VulkanRenderPass *renderPass)
{
    auto device = vulkanDevice->getDevice();

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VertexTextured);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescriptions[3];
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexTextured, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexTextured, uv);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexTextured, normal);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 3;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D extent = {512, 512};
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = reverseFaceCooling ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    if (enableColorBlending)
    {
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    }
    else
    {
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 0;
        colorBlending.pAttachments = nullptr;
    }

    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstantObject);

    if (enableSampler)
    {
        VkDescriptorSetLayout layouts[2] = {
            descriptorSetLayoutPipeline->getDescriptorSetLayout(),
            descriptorSetLayoutSampler->getDescriptorSetLayout()};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 2;
        pipelineLayoutInfo.pSetLayouts = layouts;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            std::cout << "unable to create pipeline layout" << std::endl;
            return false;
        }
    }
    else
    {
        VkDescriptorSetLayout layouts[1] = {descriptorSetLayoutPipeline->getDescriptorSetLayout()};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = layouts;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            std::cout << "unable to create pipeline layout" << std::endl;
            return false;
        }
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = enableDepthTest ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = enableDepthWrite ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = enableDepthWrite ? VK_COMPARE_OP_LESS : VK_COMPARE_OP_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = stageAmount;
    pipelineInfo.pStages = (VkPipelineShaderStageCreateInfo *)shader->getShaderStages();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    vkDeviceWaitIdle(device);

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        std::cout << "Unable to create graphics pipeline" << std::endl;
        return false;
    }
    if (graphicsPipeline == VK_NULL_HANDLE)
        std::cout << "Pipeline is null!" << std::endl;
    return true;
}
