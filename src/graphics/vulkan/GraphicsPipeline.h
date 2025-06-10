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
  const std::shared_ptr<PipelineLayout> _layout;
  const std::vector<std::shared_ptr<Shader>> _shaders;
  const VkPrimitiveTopology _primitiveTopology;
  const VkPolygonMode _polygonMode;
  const bool _depthWrite;

public:
  GraphicsPipelineSettings(std::shared_ptr<PipelineLayout> layout,
    std::vector<std::shared_ptr<Shader>> shaders,
    VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode,
    bool depthWrite);

  std::shared_ptr<PipelineLayout> getLayout() const;
  std::vector<std::shared_ptr<Shader>> getShaders() const;
  VkPrimitiveTopology getPrimitiveTopology() const;
  VkPolygonMode getPolygonMode() const;
  bool getDepthWrite() const;

  bool operator==(const GraphicsPipelineSettings& other) const;
};

class GraphicsPipeline {
private:
  VkPipeline _handle;
  const GraphicsPipelineSettings _settings;

  VkPipelineRenderingCreateInfo getRenderingCreateInfo() const;
  VkPipelineVertexInputStateCreateInfo getVertexInputState() const;
  VkPipelineInputAssemblyStateCreateInfo getInputAssemblyState() const;
  VkPipelineTessellationStateCreateInfo getTessellationState() const;
  VkPipelineViewportStateCreateInfo getViewportState() const;
  VkPipelineRasterizationStateCreateInfo getRasterizationState() const;
  VkPipelineMultisampleStateCreateInfo getMultisampleState() const;
  VkPipelineDepthStencilStateCreateInfo getDepthStencilState() const;
  PipelineColorBlendStateCreateInfoData getColorBlendState() const;
  PipelineDynamicStateCreateInfoData getDynamicState() const;

  GraphicsPipelineCreateInfoData getCreateInfo() const;

public:
  VkPipeline getHandle() const;

  GraphicsPipeline(std::shared_ptr<PipelineLayout> layout,
    std::vector<std::shared_ptr<Shader>> shaders,
    VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode,
    bool depthWrite);

  ~GraphicsPipeline();

  bool matchSettings(const GraphicsPipelineSettings& settings) const;
};