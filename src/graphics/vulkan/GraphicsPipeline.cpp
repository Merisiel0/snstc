#include "GraphicsPipeline.h"

#include "Device.h"
#include "Image.h"

// VkPushConstantRange GraphicsPipeline::getPushConstantRange() const {
//   VkPushConstantRange range {};
//   range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//   range.offset = 0;
//   range.size = sizeof(PushConstants);

//   return range;
// }

VkPipelineRenderingCreateInfo GraphicsPipeline::getRenderingCreateInfo() const {
  VkPipelineRenderingCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  info.pNext = nullptr;
  info.viewMask = 0;
  info.colorAttachmentCount = 1;
  info.pColorAttachmentFormats = &Image::colorFormat;
  info.depthAttachmentFormat = Image::depthFormat;
  info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

  return info;
}

VkPipelineVertexInputStateCreateInfo GraphicsPipeline::getVertexInputState() const {
  VkPipelineVertexInputStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.vertexBindingDescriptionCount = 0;
  state.pVertexBindingDescriptions = nullptr;
  state.vertexAttributeDescriptionCount = 0;
  state.pVertexAttributeDescriptions = nullptr;

  return state;
}

VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline::getInputAssemblyState(
  VkPrimitiveTopology primitiveTopology) const {
  VkPipelineInputAssemblyStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.topology = primitiveTopology;
  state.primitiveRestartEnable = VK_FALSE;

  return state;
}

VkPipelineTessellationStateCreateInfo GraphicsPipeline::getTessellationState() const {
  VkPipelineTessellationStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.patchControlPoints = 0;

  return state;
}

VkPipelineViewportStateCreateInfo GraphicsPipeline::getViewportState() const {
  VkPipelineViewportStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.viewportCount = 0;
  state.pViewports = nullptr;
  state.scissorCount = 0;
  state.pScissors = nullptr;

  return state;
}

VkPipelineRasterizationStateCreateInfo GraphicsPipeline::getRasterizationState(
  VkPolygonMode polygonMode) const {
  VkPipelineRasterizationStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.depthClampEnable = VK_FALSE;
  state.rasterizerDiscardEnable = VK_FALSE;
  state.polygonMode = polygonMode;
  state.cullMode = VK_CULL_MODE_BACK_BIT;
  state.frontFace = VK_FRONT_FACE_CLOCKWISE;
  state.depthBiasEnable = VK_FALSE;
  state.depthBiasConstantFactor = 0;
  state.depthBiasClamp = 0;
  state.depthBiasSlopeFactor = 0;
  state.lineWidth = 1.0f;

  return state;
}

VkPipelineMultisampleStateCreateInfo GraphicsPipeline::getMultisampleState() const {
  VkPipelineMultisampleStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  state.sampleShadingEnable = VK_FALSE;
  state.minSampleShading = 0;
  state.pSampleMask = nullptr;
  state.alphaToCoverageEnable = VK_FALSE;
  state.alphaToOneEnable = VK_FALSE;

  return state;
}

VkPipelineDepthStencilStateCreateInfo GraphicsPipeline::getDepthStencilState(
  bool depthWrite) const {
  VkPipelineDepthStencilStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.depthTestEnable = VK_TRUE;
  state.depthWriteEnable = (VkBool32) depthWrite;
  state.depthCompareOp = VK_COMPARE_OP_LESS;
  state.depthBoundsTestEnable = VK_FALSE;
  state.stencilTestEnable = VK_FALSE;
  state.front = {};
  state.back = {};
  state.minDepthBounds = 0.0f;
  state.maxDepthBounds = 1.0f;

  return state;
}

PipelineColorBlendStateCreateInfoData GraphicsPipeline::getColorBlendState() const {
  PipelineColorBlendStateCreateInfoData data;
  VkPipelineColorBlendAttachmentState attachment {};
  attachment.blendEnable = VK_FALSE;
  attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  attachment.colorBlendOp = VK_BLEND_OP_ADD;
  attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  attachment.alphaBlendOp = VK_BLEND_OP_ADD;
  attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  data.attachments.push_back(attachment);

  data.info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  data.info.pNext = nullptr;
  data.info.flags = 0;
  data.info.logicOpEnable = VK_FALSE;
  data.info.logicOp = VK_LOGIC_OP_COPY;
  data.info.attachmentCount = data.attachments.size();
  data.info.pAttachments = data.attachments.data();
  data.info.blendConstants[0] = 0;
  data.info.blendConstants[1] = 0;
  data.info.blendConstants[2] = 0;
  data.info.blendConstants[3] = 0;

  return data;
}

PipelineDynamicStateCreateInfoData GraphicsPipeline::getDynamicState() const {
  PipelineDynamicStateCreateInfoData data;
  data.states = {
    VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT, // allows for resizing of window
    VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT, // allows for resizing of window
    VK_DYNAMIC_STATE_LINE_WIDTH, // allows custom line width
    VK_DYNAMIC_STATE_CULL_MODE, // to let the user decide
  };

  data.info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  data.info.pNext = nullptr;
  data.info.flags = 0;
  data.info.dynamicStateCount = (uint32_t) data.states.size();
  data.info.pDynamicStates = data.states.data();

  return data;
}

GraphicsPipelineCreateInfoData GraphicsPipeline::getCreateInfo(
  std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
  VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode, bool depthWrite) const {
  GraphicsPipelineCreateInfoData data;

  // get necessary informations
  data.rendering = getRenderingCreateInfo();
  data.vertexInput = getVertexInputState();
  data.inputAssembly = getInputAssemblyState(primitiveTopology);
  data.tessellation = getTessellationState();
  data.viewport = getViewportState();
  data.rasterization = getRasterizationState(polygonMode);
  data.multisample = getMultisampleState();
  data.depthStencil = getDepthStencilState(depthWrite);
  data.colorBlend = getColorBlendState();
  data.dynamicState = getDynamicState();

  // fill pipeline create info
  data.info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  data.info.pNext = &data.rendering;
  data.info.flags = 0;

  data.info.stageCount = (uint32_t) shaderStageCreateInfos.size();
  data.info.pStages = shaderStageCreateInfos.data();

  data.info.pVertexInputState = &data.vertexInput;
  data.info.pInputAssemblyState = &data.inputAssembly;
  data.info.pTessellationState = &data.tessellation;
  data.info.pViewportState = &data.viewport;
  data.info.pRasterizationState = &data.rasterization;
  data.info.pMultisampleState = &data.multisample;
  data.info.pDepthStencilState = &data.depthStencil;
  data.info.pColorBlendState = &data.colorBlend.info;
  data.info.pDynamicState = &data.dynamicState.info;
  data.info.layout = _layout;
  data.info.renderPass = VK_NULL_HANDLE;
  data.info.subpass = 0;
  data.info.basePipelineHandle = VK_NULL_HANDLE;
  data.info.basePipelineIndex = 0;

  return data;
}

GraphicsPipeline::GraphicsPipeline(std::shared_ptr<Device> device,
  VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode,
  std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
  std::vector<VkPushConstantRange> pushConstantRanges,
  std::vector<VkDescriptorSetLayout>& setLayouts, bool depthWrite) :
    IPipeline(device, pushConstantRanges, setLayouts) {
  _type = VK_PIPELINE_BIND_POINT_GRAPHICS;

  GraphicsPipelineCreateInfoData data =
    getCreateInfo(shaderStageCreateInfos, primitiveTopology, polygonMode, depthWrite);
  VK_CHECK(
    vkCreateGraphicsPipelines(device->handle, VK_NULL_HANDLE, 1, &data.info, nullptr, &handle));
}
