#pragma once

#include "VulkanUtils.h"

class Device;

class IPipeline {
private:
  VkPipelineLayoutCreateInfo getLayoutCreateInfo(
    std::vector<VkPushConstantRange>& pushConstantRanges,
    std::vector<VkDescriptorSetLayout>& setLayouts) const;

protected:
  std::shared_ptr<Device> _device;
  VkPipelineLayout _layout;
  VkPipelineBindPoint _type;

public:
  VkPipeline handle;

  IPipeline(std::shared_ptr<Device> device, std::vector<VkPushConstantRange> pushConstantRanges,
    std::vector<VkDescriptorSetLayout>& setLayouts);
  ~IPipeline();

  VkPipelineLayout layout() const { return _layout; }
  VkPipelineBindPoint type() const { return _type; }
};