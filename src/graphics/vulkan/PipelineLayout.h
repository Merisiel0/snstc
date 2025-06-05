#pragma once

#include "DescriptorManager.h"
#include "VulkanUtils.h"

struct PipelineLayoutSettings {
private:
  std::vector<DescriptorSetLayoutType> _setLayouts;
  std::vector<VkPushConstantRange> _pushConstantRanges;

public:
  PipelineLayoutSettings() {};

  PipelineLayoutSettings(std::vector<DescriptorSetLayoutType> _setLayouts,
    std::vector<VkPushConstantRange> _pushConstantRanges);

  std::vector<DescriptorSetLayoutType> getSetLayouts() const;
  std::vector<VkPushConstantRange> getPushConstantRanges() const;

  bool operator==(const PipelineLayoutSettings& other) const;
};

class PipelineLayout {
private:
  VkPipelineLayout _handle;
  PipelineLayoutSettings _settings;

  VkPipelineLayoutCreateInfo getCreateInfo() const;

public:
  VkPipelineLayout getHandle() const;

  PipelineLayout(const std::vector<DescriptorSetLayoutType>& setLayouts,
    const std::vector<VkPushConstantRange>& pushConstantRanges);
  ~PipelineLayout();

  const PipelineLayoutSettings& getSettings() const;
  bool matchSettings(PipelineLayoutSettings settings) const;
  bool hasSetLayout(DescriptorSetLayoutType layout) const;
};