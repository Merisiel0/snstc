#include "GraphicsPipeline.h"

#include "Device.h"
#include "Image.h"

VkPushConstantRange GraphicsPipeline::getPushConstantRange() const {
  VkPushConstantRange range {};
  range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  //range.offset = 0;
  range.size = sizeof(PushConstants);

  return range;
}

VkPipelineRenderingCreateInfo GraphicsPipeline::getRenderingCreateInfo() const {
  VkPipelineRenderingCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  //info.pNext = nullptr;
  //info.viewMask = 0;
  info.colorAttachmentCount = 1;
  info.pColorAttachmentFormats = &Image::colorFormat;
  info.depthAttachmentFormat = Image::depthFormat;
  info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

  return info;
}

VkPipelineVertexInputStateCreateInfo GraphicsPipeline::getVertexInputState() const {
  VkPipelineVertexInputStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  //state.vertexBindingDescriptionCount = 0;
  //state.pVertexBindingDescriptions = nullptr;
  //state.vertexAttributeDescriptionCount = 0;
  //state.pVertexAttributeDescriptions = nullptr;

  return state;
}

VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline::getInputAssemblyState(
  VkPrimitiveTopology primitiveTopology) const {
  VkPipelineInputAssemblyStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  state.topology = primitiveTopology;
  state.primitiveRestartEnable = VK_FALSE;

  return state;
}

VkPipelineTessellationStateCreateInfo GraphicsPipeline::getTessellationState() const {
  VkPipelineTessellationStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  //state.patchControlPoints = 0;

  return state;
}

VkPipelineViewportStateCreateInfo GraphicsPipeline::getViewportState() const {
  VkPipelineViewportStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  //state.viewportCount = 0;
  //state.pViewports = nullptr;
  //state.scissorCount = 0;
  //state.pScissors = nullptr;

  return state;
}

VkPipelineRasterizationStateCreateInfo GraphicsPipeline::getRasterizationState(
  VkPolygonMode polygonMode) const {
  VkPipelineRasterizationStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  state.depthClampEnable = VK_FALSE;
  state.rasterizerDiscardEnable = VK_FALSE;
  state.polygonMode = polygonMode;
  state.cullMode = VK_CULL_MODE_BACK_BIT;
  state.frontFace = VK_FRONT_FACE_CLOCKWISE;
  state.depthBiasEnable = VK_FALSE;
  //state.depthBiasConstantFactor = 0;
  //state.depthBiasClamp = 0;
  //state.depthBiasSlopeFactor = 0;
  state.lineWidth = 1.0f;

  return state;
}

VkPipelineMultisampleStateCreateInfo GraphicsPipeline::getMultisampleState() const {
  VkPipelineMultisampleStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  state.sampleShadingEnable = VK_FALSE;
  //state.minSampleShading = 0;
  //state.pSampleMask = nullptr;
  state.alphaToCoverageEnable = VK_FALSE;
  state.alphaToOneEnable = VK_FALSE;

  return state;
}

VkPipelineDepthStencilStateCreateInfo GraphicsPipeline::getDepthStencilState() const {
  VkPipelineDepthStencilStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  //state.pNext;
  state.flags;
  state.depthTestEnable = VK_TRUE;
  state.depthWriteEnable = VK_TRUE;
  state.depthCompareOp = VK_COMPARE_OP_LESS;
  //state.depthBoundsTestEnable = VK_FALSE;
  //state.stencilTestEnable = VK_FALSE;
  //state.front;
  //state.back;
  //state.minDepthBounds = 0.0f;
  //state.maxDepthBounds = 1.0f;

  return state;
}

VkPipelineColorBlendStateCreateInfo GraphicsPipeline::getColorBlendState() const {
  VkPipelineColorBlendAttachmentState colorBlendAttachment {};
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  state.logicOpEnable = VK_FALSE;
  state.logicOp = VK_LOGIC_OP_COPY;
  state.attachmentCount = 1;
  state.pAttachments = new VkPipelineColorBlendAttachmentState(colorBlendAttachment);
  //state.blendConstants[0] = 0;
  //state.blendConstants[1] = 0;
  //state.blendConstants[2] = 0;
  //state.blendConstants[3] = 0;

  return state;
}

VkPipelineDynamicStateCreateInfo GraphicsPipeline::getDynamicState() const {
  VkPipelineDynamicStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  //state.pNext = nullptr;
  //state.flags = 0;
  state.dynamicStateCount = 4;
  state.pDynamicStates = new VkDynamicState[4] {
    VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT, // allows for resizing of window
    VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT, // allows for resizing of window
    VK_DYNAMIC_STATE_LINE_WIDTH, // allows custom line width
    VK_DYNAMIC_STATE_CULL_MODE, // to let the user decide
  };

  return state;
}

VkGraphicsPipelineCreateInfo GraphicsPipeline::getCreateInfo(
  std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos,
  VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode) const {
  VkGraphicsPipelineCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.pNext = new VkPipelineRenderingCreateInfo(getRenderingCreateInfo());
  //info.flags = 0;
  info.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());

  VkPipelineShaderStageCreateInfo* stagesPtr = new VkPipelineShaderStageCreateInfo[info.stageCount];
  std::copy(shaderStageCreateInfos.begin(), shaderStageCreateInfos.end(), stagesPtr);
  info.pStages = stagesPtr;

  info.pVertexInputState = new VkPipelineVertexInputStateCreateInfo(getVertexInputState());
  info.pInputAssemblyState =
    new VkPipelineInputAssemblyStateCreateInfo(getInputAssemblyState(primitiveTopology));
  info.pTessellationState = new VkPipelineTessellationStateCreateInfo(getTessellationState());
  info.pViewportState = new VkPipelineViewportStateCreateInfo(getViewportState());
  info.pRasterizationState =
    new VkPipelineRasterizationStateCreateInfo(getRasterizationState(polygonMode));
  info.pMultisampleState = new VkPipelineMultisampleStateCreateInfo(getMultisampleState());
  info.pDepthStencilState = new VkPipelineDepthStencilStateCreateInfo(getDepthStencilState());
  info.pColorBlendState = new VkPipelineColorBlendStateCreateInfo(getColorBlendState());
  info.pDynamicState = new VkPipelineDynamicStateCreateInfo(getDynamicState());
  info.layout = _layout;
  info.renderPass = VK_NULL_HANDLE;
  //info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;
  //info.basePipelineIndex = 0;

  return info;
}

GraphicsPipeline::GraphicsPipeline(std::shared_ptr<Device> device,
  VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode,
  std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos,
  std::vector<VkDescriptorSetLayout> setLayouts) :
    IPipeline(device, {getPushConstantRange()}, setLayouts) {
  _type = VK_PIPELINE_BIND_POINT_GRAPHICS;

  VkGraphicsPipelineCreateInfo createInfo =
    getCreateInfo(shaderStageCreateInfos, primitiveTopology, polygonMode);
  VK_CHECK(vkCreateGraphicsPipelines(device->handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle));
}
