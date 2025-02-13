#include "Shader.h"

#include "Device.h"

VkShaderModuleCreateInfo Shader::getCreateInfo(std::vector<uint32_t> data) const {
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  //info.pNext;
  //info.flags;
  info.codeSize = data.size() * sizeof(uint32_t);
  uint32_t* dataPtr = new uint32_t[data.size()];
  std::copy(data.begin(), data.end(), dataPtr);
  info.pCode = dataPtr;

  return info;
}

Shader::Shader(Device* device, const char* path, VkShaderStageFlagBits stage) {
  _devicePtr = &device->handle;
  _stage = stage;

  std::vector<uint32_t> data = readFileBytes(path);

  VkShaderModuleCreateInfo moduleCreateInfo = getCreateInfo(data);
  vkCreateShaderModule(device->handle, &moduleCreateInfo, nullptr, &handle);
}

Shader::~Shader() {
  vkDestroyShaderModule(*_devicePtr, handle, nullptr);
}

VkPipelineShaderStageCreateInfo Shader::getStageCreateInfo() const {
  VkPipelineShaderStageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  //info.pNext;
  //info.flags;
  info.stage = _stage;
  info.module = handle;
  info.pName = "main";
  //info.pSpecializationInfo;

  return info;
}
