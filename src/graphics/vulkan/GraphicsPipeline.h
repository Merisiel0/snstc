#pragma once

#include "Pipeline.h"
#include "VulkanUtils.h"
#include "utils/FunctionQueue.h"

class Device;

struct PushConstants {
  mat4 model;
  VkDeviceAddress vertexBuffer;
};

class GraphicsPipeline : public IPipeline {
private:
  VkPushConstantRange getPushConstantRange() const;

  VkPipelineRenderingCreateInfo getRenderingCreateInfo() const;
  VkPipelineVertexInputStateCreateInfo getVertexInputState() const;
  VkPipelineInputAssemblyStateCreateInfo getInputAssemblyState(
    VkPrimitiveTopology primitiveTopology) const;
  VkPipelineTessellationStateCreateInfo getTessellationState() const;
  VkPipelineViewportStateCreateInfo getViewportState() const;
  VkPipelineRasterizationStateCreateInfo getRasterizationState(VkPolygonMode polygonMode) const;
  VkPipelineMultisampleStateCreateInfo getMultisampleState() const;
  VkPipelineDepthStencilStateCreateInfo getDepthStencilState(bool depthWrite) const;
  PipelineColorBlendStateCreateInfoData getColorBlendState() const;
  PipelineDynamicStateCreateInfoData getDynamicState() const;

  GraphicsPipelineCreateInfoData getCreateInfo(
    std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
    VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode, bool depthWrite) const;

public:
  GraphicsPipeline(std::shared_ptr<Device>, VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode, std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
    std::vector<VkDescriptorSetLayout>& setLayouts, bool depthWrite = true);
};