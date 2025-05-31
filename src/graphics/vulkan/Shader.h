#pragma once

#include "VulkanUtils.h"

class Device;

class Shader {
private:
  VkShaderStageFlagBits _stage;

  VkShaderModuleCreateInfo getCreateInfo(std::vector<uint32_t>& data) const;

public:
  VkShaderModule handle;

  Shader(std::string path, VkShaderStageFlagBits stage);
  ~Shader();

  Shader(const Shader& other) = delete;
  Shader& operator=(const Shader& other) = delete;

  Shader(Shader&& other);
  Shader& operator=(Shader&& other);

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;
};