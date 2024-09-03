#include "GraphicsPipeline.h"

#include "Device.h"

GraphicsPipeline::GraphicsPipeline(Device* device, VkFormat colorAttachmentFormat,
  std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos, VkPrimitiveTopology primitiveTopology)
  : IPipeline(device) {

  VkPipelineRenderingCreateInfo renderInfo{};
  renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  //renderInfo.pNext = nullptr;
  //renderInfo.viewMask = 0;
  renderInfo.colorAttachmentCount = 1;
  renderInfo.pColorAttachmentFormats = &colorAttachmentFormat;
  renderInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
  renderInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  //vertexInputState.pNext = nullptr;
  //vertexInputState.flags = 0;
  //vertexInputState.vertexBindingDescriptionCount = 0;
  //vertexInputState.pVertexBindingDescriptions = nullptr;
  //vertexInputState.vertexAttributeDescriptionCount = 0;
  //vertexInputState.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
  inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  //inputAssemblyState.pNext = nullptr;
  //inputAssemblyState.flags = 0;
  inputAssemblyState.topology = primitiveTopology;
  inputAssemblyState.primitiveRestartEnable = VK_FALSE;

  VkPipelineTessellationStateCreateInfo tesselationState{};
  tesselationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  //tesselationState.pNext = nullptr;
  //tesselationState.flags = 0;
  //tesselationState.patchControlPoints = 0;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  //viewportState.pNext = nullptr;
  //viewportState.flags = 0;
  //viewportState.viewportCount = 0;
  //viewportState.pViewports = nullptr;
  //viewportState.scissorCount = 0;
  //viewportState.pScissors = nullptr;

  VkPipelineRasterizationStateCreateInfo rasterizationState{};
  rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  //rasterizationState.pNext = nullptr;
  //rasterizationState.flags = 0;
  rasterizationState.depthClampEnable = VK_FALSE;
  rasterizationState.rasterizerDiscardEnable = VK_FALSE;
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.cullMode = VK_CULL_MODE_NONE;
  rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizationState.depthBiasEnable = VK_FALSE;
  //rasterizationState.depthBiasConstantFactor = 0;
  //rasterizationState.depthBiasClamp = 0;
  //rasterizationState.depthBiasSlopeFactor = 0;
  rasterizationState.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampleState{};
  multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  //multisampleState.pNext = nullptr;
  //multisampleState.flags = 0;
  multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleState.sampleShadingEnable = VK_FALSE;
  //multisampleState.minSampleShading = 0;
  //multisampleState.pSampleMask = nullptr;
  multisampleState.alphaToCoverageEnable = VK_FALSE;
  multisampleState.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
    | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendState{};
  colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  //colorBlendState.pNext = nullptr;
  //colorBlendState.flags = 0;
  colorBlendState.logicOpEnable = VK_TRUE;
  colorBlendState.logicOp = VK_LOGIC_OP_COPY;
  colorBlendState.attachmentCount = 1;
  colorBlendState.pAttachments = &colorBlendAttachment;
  //colorBlendState.blendConstants[0] = 0;
  //colorBlendState.blendConstants[1] = 0;
  //colorBlendState.blendConstants[2] = 0;
  //colorBlendState.blendConstants[3] = 0;

  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT, // allows for resizing of window
    VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT, // allows for resizing of window
    VK_DYNAMIC_STATE_LINE_WIDTH, // allows custom line width
    VK_DYNAMIC_STATE_CULL_MODE, // to let the user decide
  };

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  //dynamicState.pNext = nullptr;
  //dynamicState.flags = 0;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PushConstants);

  VkPipelineLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  //layoutInfo.pNext = nullptr;
  //layoutInfo.flags = 0;
  //layoutInfo.setLayoutCount = 0;
  //layoutInfo.pSetLayouts = nullptr;
  layoutInfo.pushConstantRangeCount = 1;
  layoutInfo.pPushConstantRanges = &pushConstantRange;

  VK_CHECK(vkCreatePipelineLayout(device->handle, &layoutInfo, nullptr, &_layout));

  // --- CREATE INFO ---
  VkGraphicsPipelineCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  createInfo.pNext = &renderInfo;
  //createInfo.flags;
  createInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
  createInfo.pStages = shaderStageCreateInfos.data();
  createInfo.pVertexInputState = &vertexInputState;
  createInfo.pInputAssemblyState = &inputAssemblyState;
  createInfo.pTessellationState = &tesselationState;
  createInfo.pViewportState = &viewportState;
  createInfo.pRasterizationState = &rasterizationState;
  createInfo.pMultisampleState = &multisampleState;
  createInfo.pDepthStencilState = nullptr;
  createInfo.pColorBlendState = &colorBlendState;
  createInfo.pDynamicState = &dynamicState;
  createInfo.layout = _layout;
  //createInfo.renderPass;
  //createInfo.subpass;
  //createInfo.basePipelineHandle;
  //createInfo.basePipelineIndex;

  vkCreateGraphicsPipelines(device->handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle);
}
