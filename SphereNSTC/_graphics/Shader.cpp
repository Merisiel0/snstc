#include "Shader.h"

#include "Device.h"

Shader::Shader(Device* device, const char* path, VkShaderStageFlagBits stage) {
  _devicePtr = &device->handle;
  _stage = stage;

  std::vector<uint32_t> data = readFileBytes(path);

  VkShaderModuleCreateInfo moduleCreateInfo{};
  moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  //moduleCreateInfo.pNext;
  //moduleCreateInfo.flags;
  moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
  moduleCreateInfo.pCode = data.data();

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
