#pragma once

#include "VulkanUtils.h"

class Device;

class Shader {
private:
  VkDevice* _devicePtr{ nullptr };
  VkShaderStageFlagBits _stage{};

public:
  VkShaderModule handle{ VK_NULL_HANDLE };

  Shader() {};
  Shader(Device* device, const char* path, VkShaderStageFlagBits stage);
  ~Shader();

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;
};