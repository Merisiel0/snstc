#include "GraphicsPipeline.h"

#include "Device.h"
#include "Image.h"
#include "PipelineLayout.h"
#include "Shader.h"
#include "VulkanHandler.h"

GraphicsPipelineSettings::GraphicsPipelineSettings(std::shared_ptr<PipelineLayout> layout,
  std::vector<std::shared_ptr<Shader>> shaders,
  VkPrimitiveTopology primitiveTopology,
  VkPolygonMode polygonMode,
  bool depthWrite) :
    _layout {layout},
    _shaders {shaders},
    _primitiveTopology {primitiveTopology},
    _polygonMode {polygonMode},
    _depthWrite {depthWrite} {}

std::shared_ptr<PipelineLayout> GraphicsPipelineSettings::getLayout() const { return _layout; }

std::vector<std::shared_ptr<Shader>> GraphicsPipelineSettings::getShaders() const {
  return _shaders;
}

VkPrimitiveTopology GraphicsPipelineSettings::getPrimitiveTopology() const {
  return _primitiveTopology;
}

VkPolygonMode GraphicsPipelineSettings::getPolygonMode() const { return _polygonMode; }

bool GraphicsPipelineSettings::getDepthWrite() const { return _depthWrite; }

bool GraphicsPipelineSettings::operator==(const GraphicsPipelineSettings& other) const {
  bool shadersMatch = _shaders.size() == other._shaders.size();

  if(!shadersMatch) return false;

  int matchingShaders = 0;
  for(const auto& shaderA : _shaders) {
    for(const auto& shaderB : other._shaders) {
      if(shaderA == shaderB) matchingShaders++;
    }
  }

  if(matchingShaders != _shaders.size()) return false;

  bool layoutMatch = _layout == other._layout;

  if(!layoutMatch) return false;

  return _primitiveTopology == other._primitiveTopology && _polygonMode == other._polygonMode &&
         _depthWrite == other._depthWrite;
}

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

VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline::getInputAssemblyState() const {
  VkPipelineInputAssemblyStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.topology = _settings.getPrimitiveTopology();
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

VkPipelineRasterizationStateCreateInfo GraphicsPipeline::getRasterizationState() const {
  VkPipelineRasterizationStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.depthClampEnable = VK_FALSE;
  state.rasterizerDiscardEnable = VK_FALSE;
  state.polygonMode = _settings.getPolygonMode();
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

VkPipelineDepthStencilStateCreateInfo GraphicsPipeline::getDepthStencilState() const {
  VkPipelineDepthStencilStateCreateInfo state {};
  state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  state.pNext = nullptr;
  state.flags = 0;
  state.depthTestEnable = VK_TRUE;
  state.depthWriteEnable = (VkBool32) _settings.getDepthWrite();
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

GraphicsPipelineCreateInfoData GraphicsPipeline::getCreateInfo() const {
  GraphicsPipelineCreateInfoData data;

  // get necessary informations
  data.rendering = getRenderingCreateInfo();
  data.vertexInput = getVertexInputState();
  data.inputAssembly = getInputAssemblyState();
  data.tessellation = getTessellationState();
  data.viewport = getViewportState();
  data.rasterization = getRasterizationState();
  data.multisample = getMultisampleState();
  data.depthStencil = getDepthStencilState();
  data.colorBlend = getColorBlendState();
  data.dynamicState = getDynamicState();

  // fill pipeline create info
  data.info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  data.info.pNext = &data.rendering;
  data.info.flags = 0;

  std::vector<VkPipelineShaderStageCreateInfo> shaderStages {};
  for(std::shared_ptr<Shader> shader : _settings.getShaders()) {
    shaderStages.push_back(shader->getStageCreateInfo());
  }
  data.info.stageCount = (uint32_t) shaderStages.size();
  data.info.pStages = shaderStages.data();

  data.info.pVertexInputState = &data.vertexInput;
  data.info.pInputAssemblyState = &data.inputAssembly;
  data.info.pTessellationState = &data.tessellation;
  data.info.pViewportState = &data.viewport;
  data.info.pRasterizationState = &data.rasterization;
  data.info.pMultisampleState = &data.multisample;
  data.info.pDepthStencilState = &data.depthStencil;
  data.info.pColorBlendState = &data.colorBlend.info;
  data.info.pDynamicState = &data.dynamicState.info;
  data.info.layout = _settings.getLayout()->getHandle();
  ;
  data.info.renderPass = VK_NULL_HANDLE;
  data.info.subpass = 0;
  data.info.basePipelineHandle = VK_NULL_HANDLE;
  data.info.basePipelineIndex = 0;

  return data;
}

VkPipeline GraphicsPipeline::getHandle() const { return _handle; }

std::shared_ptr<GraphicsPipeline> GraphicsPipeline::load(const GraphicsPipelineSettings settings) {
  std::shared_ptr<GraphicsPipeline> sptr = findResource(settings);
  if(sptr) return sptr;

  sptr = std::make_shared<GraphicsPipeline>(settings);

  addResource(settings, sptr);

  return sptr;
}

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineSettings settings) : _settings {settings} {
  GraphicsPipelineCreateInfoData data = getCreateInfo();

  VK_CHECK(vkCreateGraphicsPipelines(VulkanHandler::getDevice()->handle, VK_NULL_HANDLE, 1,
    &data.info, nullptr, &_handle));
}

GraphicsPipeline::~GraphicsPipeline() {
  vkDestroyPipeline(VulkanHandler::getDevice()->handle, _handle, nullptr);
}

bool GraphicsPipeline::matchSettings(const GraphicsPipelineSettings& settings) const {
  return _settings == settings;
}
