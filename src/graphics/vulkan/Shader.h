#pragma once

#include "VulkanUtils.h"

class Device;

class Shader {
private:
  std::shared_ptr<Device> _device;
  VkShaderStageFlagBits _stage;

  VkShaderModuleCreateInfo getCreateInfo(std::vector<uint32_t>& data) const;

public:
  VkShaderModule handle;

  Shader(std::shared_ptr<Device> device, const char* path, VkShaderStageFlagBits stage);
  ~Shader();

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;
};