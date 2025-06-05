#pragma once

#include "VulkanUtils.h"
#include "utils/FunctionQueue.h"

class PipelineLayout;
class Shader;

struct PushConstants {
  mat4 model;
  VkDeviceAddress vertexBuffer;
};

struct GraphicsPipelineSettings {
private:
  std::shared_ptr<PipelineLayout> _layout;
  std::vector<std::string> _shaderPaths;
  VkPrimitiveTopology _primitiveTopology;
  VkPolygonMode _polygonMode;
  bool _depthWrite;

public:
  GraphicsPipelineSettings() {};

  GraphicsPipelineSettings(std::shared_ptr<PipelineLayout> layout,
    const std::vector<Shader>& shaders,
    VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode,
    bool depthWrite);

  bool operator==(const GraphicsPipelineSettings& other) const;
};

class GraphicsPipeline {
private:
  VkPipeline _handle;
  GraphicsPipelineSettings _settings;

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

  GraphicsPipelineCreateInfoData getCreateInfo(const PipelineLayout& layout,
    const std::vector<Shader>& shaders,
    VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode,
    bool depthWrite) const;

public:
  VkPipeline getHandle() const;

  GraphicsPipeline(std::shared_ptr<PipelineLayout> layout,
    const std::vector<Shader>& shaders,
    VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode,
    bool depthWrite);

  ~GraphicsPipeline();

  bool matchSettings(const GraphicsPipelineSettings& settings) const;
};