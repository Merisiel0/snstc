#pragma once

#include "VulkanUtils.h"

class Device;

class Shader {
private:
  VkDevice* _devicePtr;
  VkShaderStageFlagBits _stage;

  VkShaderModuleCreateInfo getCreateInfo(std::vector<uint32_t> data) const;

public:
  VkShaderModule handle;

  Shader(Device* device, const char* path, VkShaderStageFlagBits stage);
  ~Shader();

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;
};