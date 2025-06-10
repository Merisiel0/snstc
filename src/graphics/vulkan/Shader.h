#pragma once

#include "VulkanUtils.h"
#include "resources/Resource.h"

class Device;

class Shader : public Resource<Shader, std::string> {
private:
  VkShaderModule _handle;
  VkShaderStageFlagBits _stage;
  std::string _path;

  VkShaderModuleCreateInfo getCreateInfo(std::vector<uint32_t>& data) const;

  Shader(std::string path, VkShaderStageFlagBits stage);

public:
  /// @brief Loads shader from a given path.
  /// @param path a path to a shader asset.
  /// @param stage a pipeline stage in which to use the shader.
  /// @return a shared pointer to a shader if the path exists, nullptr otherwise.
  static std::shared_ptr<Shader> load(std::string path, VkShaderStageFlagBits stage);

  ~Shader();

  const VkShaderModule& getHandle() const;
  std::string getPath() const;

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;

  bool operator==(const Shader& other);
};