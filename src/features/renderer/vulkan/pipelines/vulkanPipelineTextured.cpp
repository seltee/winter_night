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

bool VulkanPipelineTextured::setupColor(
    VulkanRenderPass *renderPass,
    VulkanDescriptorPool *vulkanDescriptorPool,
    VulkanObjectBuffers *vulkanObjectBuffers)
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

    if (!buildPipeline(2, true, false, true, renderPass, vulkanDescriptorPool, vulkanObjectBuffers))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupDepth(
    VulkanRenderPass *depthPass,
    VulkanDescriptorPool *vulkanDescriptorPool,
    VulkanObjectBuffers *vulkanObjectBuffers)
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

    if (!buildPipeline(1, false, true, false, depthPass, vulkanDescriptorPool, vulkanObjectBuffers))
    {
        std::cout << "Unable to build pipeline" << std::endl;
        return false;
    }

    return true;
}

void VulkanPipelineTextured::updateDescriptorSetColor(VulkanObjectBuffers *vulkanObjectBuffers)
{
    auto device = vulkanDevice->getDevice();

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = vulkanObjectBuffers->getMVPMatricesBuffer();
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferModelInfo{};
    bufferModelInfo.buffer = vulkanObjectBuffers->getModelMatricesBuffer();
    bufferModelInfo.offset = 0;
    bufferModelInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferNormalInfo{};
    bufferNormalInfo.buffer = vulkanObjectBuffers->getNormalMatricesBuffer();
    bufferNormalInfo.offset = 0;
    bufferNormalInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferGlobalDataInfo{};
    bufferGlobalDataInfo.buffer = vulkanObjectBuffers->getGlobalDataBuffer();
    bufferGlobalDataInfo.offset = 0;
    bufferGlobalDataInfo.range = vulkanObjectBuffers->getGlobalDataSize();

    VkDescriptorBufferInfo bufferLightsInfo{};
    bufferLightsInfo.buffer = vulkanObjectBuffers->getLightsDataBuffer();
    bufferLightsInfo.offset = 0;
    bufferLightsInfo.range = vulkanObjectBuffers->getLightsBufferSize();

    std::array<VkWriteDescriptorSet, 5> writes{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[1].descriptorCount = 1;
    writes[1].pBufferInfo = &bufferModelInfo;

    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstSet = descriptorSet;
    writes[2].dstBinding = 2;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[2].descriptorCount = 1;
    writes[2].pBufferInfo = &bufferNormalInfo;

    writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[3].dstSet = descriptorSet;
    writes[3].dstBinding = 3;
    writes[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[3].descriptorCount = 1;
    writes[3].pBufferInfo = &bufferGlobalDataInfo;

    writes[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[4].dstSet = descriptorSet;
    writes[4].dstBinding = 4;
    writes[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[4].descriptorCount = 1;
    writes[4].pBufferInfo = &bufferLightsInfo;

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);
}

void VulkanPipelineTextured::updateDescriptorSetDepth(VulkanObjectBuffers *vulkanObjectBuffers)
{
    auto device = vulkanDevice->getDevice();

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = vulkanObjectBuffers->getMVPMatricesBuffer();
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    std::array<VkWriteDescriptorSet, 1> writes{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);
}

VkDescriptorSetLayout VulkanPipelineTextured::getDescriptorSetLayoutPipeline()
{
    return descriptorSetLayoutPipeline->getDescriptorSetLayout();
}

VkDescriptorSetLayout VulkanPipelineTextured::getDescriptorSetLayoutSampler()
{
    return descriptorSetLayoutSampler->getDescriptorSetLayout();
}

VkDescriptorSet VulkanPipelineTextured::getDescriptorSet()
{
    return descriptorSet;
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

bool VulkanPipelineTextured::buildPipeline(
    uint32 stageAmount,
    bool enableColorBlending,
    bool enableDepthWrite,
    bool enableSampler,
    VulkanRenderPass *renderPass,
    VulkanDescriptorPool *vulkanDescriptorPool,
    VulkanObjectBuffers *vulkanObjectBuffers)
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
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
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

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = layouts;

        if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            std::cout << "unable to create descriptor set" << std::endl;
            return false;
        }
        updateDescriptorSetColor(vulkanObjectBuffers);
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

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = layouts;

        if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            std::cout << "unable to create descriptor set" << std::endl;
            return false;
        }
        updateDescriptorSetDepth(vulkanObjectBuffers);
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
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
