#include "Shader.h"

#include "Device.h"

VkShaderModuleCreateInfo Shader::getCreateInfo(std::vector<uint32_t>& data) const {
  VkShaderModuleCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  //info.pNext;
  //info.flags;
  info.codeSize = data.size() * sizeof(uint32_t);
  info.pCode = data.data();

  return info;
}

Shader::Shader(std::shared_ptr<Device> device, std::string path, VkShaderStageFlagBits stage) {
  _device = device;
  _stage = stage;

  std::vector<uint32_t> data = readFileBytes(path.c_str());

  VkShaderModuleCreateInfo moduleCreateInfo = getCreateInfo(data);
  VK_CHECK(vkCreateShaderModule(device->handle, &moduleCreateInfo, nullptr, &handle));
}

Shader::~Shader() { vkDestroyShaderModule(_device->handle, handle, nullptr); }

VkPipelineShaderStageCreateInfo Shader::getStageCreateInfo() const {
  VkPipelineShaderStageCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  //info.pNext;
  //info.flags;
  info.stage = _stage;
  info.module = handle;
  info.pName = "main";
  //info.pSpecializationInfo;

  return info;
}
