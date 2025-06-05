#include "Shader.h"

#include "Device.h"
#include "VulkanHandler.h"

VkShaderModuleCreateInfo Shader::getCreateInfo(std::vector<uint32_t>& data) const {
  VkShaderModuleCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.codeSize = data.size() * sizeof(uint32_t);
  info.pCode = data.data();

  return info;
}

Shader::Shader(std::string path, VkShaderStageFlagBits stage) {
  _stage = stage;
  _path = path;

  std::vector<uint32_t> data = readFileBytes(path.c_str());

  VkShaderModuleCreateInfo moduleCreateInfo = getCreateInfo(data);
  VK_CHECK(
    vkCreateShaderModule(VulkanHandler::getDevice()->handle, &moduleCreateInfo, nullptr, &handle));
}

Shader::~Shader() { vkDestroyShaderModule(VulkanHandler::getDevice()->handle, handle, nullptr); }

Shader::Shader(Shader&& other) : _stage {other._stage}, handle {other.handle} {
  other.handle = VK_NULL_HANDLE;
}

Shader& Shader::operator=(Shader&& other) {
  _stage = other._stage;
  handle = other.handle;

  other.handle = VK_NULL_HANDLE;

  return *this;
}

std::string Shader::getPath() const { return _path; }

VkPipelineShaderStageCreateInfo Shader::getStageCreateInfo() const {
  VkPipelineShaderStageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.stage = _stage;
  info.module = handle;
  info.pName = "main";
  info.pSpecializationInfo = nullptr;

  return info;
}
