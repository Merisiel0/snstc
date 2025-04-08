#pragma once

#include "VulkanUtils.h"

class Device;

class DescriptorSetLayout {
private:
  std::shared_ptr<Device> _device;

  VkDescriptorSetLayoutCreateInfo getCreateInfo(
    std::vector<VkDescriptorSetLayoutBinding> bindings) const;

public:
  VkDescriptorSetLayout handle;

  DescriptorSetLayout(std::shared_ptr<Device> device,
    std::vector<VkDescriptorSetLayoutBinding> bindings);
  ~DescriptorSetLayout();
};