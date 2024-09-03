#pragma once

#include "VulkanUtils.h"

#include "Pipeline.h"

class Device;
class Shader;

struct PushConstants {
  glm::mat4 transform;
  VkDeviceAddress vertexBuffer;
};

class GraphicsPipeline : public IPipeline {
public:
  GraphicsPipeline(Device* device, VkFormat colorAttachmentFormat,
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos, VkPrimitiveTopology primitiveTopolog);
};