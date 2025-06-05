#include "PipelineLayout.h"

#include "DescriptorManager.h"
#include "Device.h"
#include "VulkanHandler.h"
#include "VulkanUtils.h"

PipelineLayoutSettings::PipelineLayoutSettings(std::vector<DescriptorSetLayoutType> setLayouts,
  std::vector<VkPushConstantRange> pushConstantRanges) {
  _setLayouts = setLayouts;
  _pushConstantRanges = pushConstantRanges;
}

std::vector<DescriptorSetLayoutType> PipelineLayoutSettings::getSetLayouts() const {
  return _setLayouts;
}

std::vector<VkPushConstantRange> PipelineLayoutSettings::getPushConstantRanges() const {
  return _pushConstantRanges;
}

bool PipelineLayoutSettings::operator==(const PipelineLayoutSettings& other) const {
  if(_setLayouts.size() != other._setLayouts.size()) { return false; };
  if(_pushConstantRanges.size() != other._pushConstantRanges.size()) { return false; };

  int matchingSets = 0;
  for(const auto& setA : _setLayouts) {
    for(const auto& setB : other._setLayouts) {
      if(setA == setB) matchingSets++;
    }
  }
  if(matchingSets != _setLayouts.size()) return false;

  int matchingPushConstants = 0;
  for(const auto& rangeA : _pushConstantRanges) {
    for(const auto& rangeB : other._pushConstantRanges) {
      if(rangeA == rangeB) matchingPushConstants++;
    }
  }
  if(matchingPushConstants != _pushConstantRanges.size()) return false;

  return true;
}

VkPipelineLayoutCreateInfo PipelineLayout::getCreateInfo() const {
  VkPipelineLayoutCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;

  std::vector<VkDescriptorSetLayout> layouts {};

  for(const auto& type : _settings.getSetLayouts()) {
    layouts.push_back(DescriptorManager::getLayout(type).getHandle());
  }

  info.setLayoutCount = (uint32_t) layouts.size();
  info.pSetLayouts = layouts.size() > 0 ? layouts.data() : nullptr;

  info.pushConstantRangeCount = (uint32_t) _settings.getPushConstantRanges().size();
  info.pPushConstantRanges = _settings.getPushConstantRanges().size() > 0
                               ? _settings.getPushConstantRanges().data()
                               : nullptr;

  return info;
}

VkPipelineLayout PipelineLayout::getHandle() const { return _handle; }

PipelineLayout::PipelineLayout(const std::vector<DescriptorSetLayoutType>& setLayouts,
  const std::vector<VkPushConstantRange>& pushConstantRanges) {
  _settings = PipelineLayoutSettings(setLayouts, pushConstantRanges);

  VkPipelineLayoutCreateInfo createInfo = getCreateInfo();
  VK_CHECK(
    vkCreatePipelineLayout(VulkanHandler::getDevice()->handle, &createInfo, nullptr, &_handle));
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(VulkanHandler::getDevice()->handle, _handle, nullptr);
}

const PipelineLayoutSettings& PipelineLayout::getSettings() const { return _settings; }

bool PipelineLayout::matchSettings(PipelineLayoutSettings settings) const {
  return _settings == settings;
}

bool PipelineLayout::hasSetLayout(DescriptorSetLayoutType layout) const {
  for(const auto& l : _settings.getSetLayouts()) {
    if(l == layout) return true;
  }

  return false;
}
