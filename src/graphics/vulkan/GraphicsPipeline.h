#pragma once

#include "VulkanUtils.h"

#include "Pipeline.h"

class Device;

struct PushConstants {
  glm::mat4 transform;
  VkDeviceAddress vertexBuffer;
};

class GraphicsPipeline : public IPipeline {
private:
  VkPushConstantRange getPushConstantRange() const;

  VkPipelineRenderingCreateInfo getRenderingCreateInfo() const;
  VkPipelineVertexInputStateCreateInfo getVertexInputState() const;
  VkPipelineInputAssemblyStateCreateInfo getInputAssemblyState(VkPrimitiveTopology primitiveTopology) const;
  VkPipelineTessellationStateCreateInfo getTessellationState() const;
  VkPipelineViewportStateCreateInfo getViewportState() const;
  VkPipelineRasterizationStateCreateInfo getRasterizationState(VkPolygonMode polygonMode) const;
  VkPipelineMultisampleStateCreateInfo getMultisampleState() const;
  VkPipelineDepthStencilStateCreateInfo getDepthStencilState() const;
  VkPipelineColorBlendStateCreateInfo getColorBlendState() const;
  VkPipelineDynamicStateCreateInfo getDynamicState() const;

  VkGraphicsPipelineCreateInfo getCreateInfo(std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos,
    VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode) const;

public:
  GraphicsPipeline(std::shared_ptr<Device>, VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode,
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos,
    std::vector<VkDescriptorSetLayout> setLayouts);
};