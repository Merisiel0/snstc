#pragma once

#include "VulkanUtils.h"

class Device;

class IPipeline {
private:
  VkPipelineLayoutCreateInfo getLayoutCreateInfo(std::vector<VkPushConstantRange> pushConstantRanges,
    std::vector<VkDescriptorSetLayout> setLayouts) const;

protected:
  VkDevice* _devicePtr;
  VkPipelineLayout _layout;
  VkPipelineBindPoint _type;

public:
  VkPipeline handle;

  IPipeline(Device* device, std::vector<VkPushConstantRange> pushConstantRanges, std::vector<VkDescriptorSetLayout> setLayouts);
  ~IPipeline();

  VkPipelineLayout layout() const { return _layout; }
  VkPipelineBindPoint type() const { return _type; }
};