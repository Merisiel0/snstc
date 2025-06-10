#include "Shader.h"

#include "Device.h"
#include "VulkanHandler.h"

#include <filesystem>

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
    vkCreateShaderModule(VulkanHandler::getDevice()->handle, &moduleCreateInfo, nullptr, &_handle));
}

std::shared_ptr<Shader> Shader::load(std::string path, VkShaderStageFlagBits stage) {
  // return material if it already exists, else generate a new one
  std::shared_ptr<Shader> sptr = findResource(path);
  if(sptr) { return sptr; }

  // check if path exists
  if(!std::filesystem::exists(path)) {
    std::clog << "Shader not found at path." << std::endl;
    return nullptr;
  }

  sptr = std::make_shared<Shader>(path, stage);
  addResource(path, sptr);

  return sptr;
}

Shader::~Shader() { vkDestroyShaderModule(VulkanHandler::getDevice()->handle, _handle, nullptr); }

const VkShaderModule& Shader::getHandle() const { return _handle; }

std::string Shader::getPath() const { return _path; }

VkPipelineShaderStageCreateInfo Shader::getStageCreateInfo() const {
  VkPipelineShaderStageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.stage = _stage;
  info.module = _handle;
  info.pName = "main";
  info.pSpecializationInfo = nullptr;

  return info;
}

bool Shader::operator==(const Shader& other) { return _handle == other._handle; }
