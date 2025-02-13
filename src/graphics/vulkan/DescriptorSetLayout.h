#pragma once

#include "VulkanUtils.h"

class Device;

class DescriptorSetLayout {
private:
  Device* _devicePtr;
  
  VkDescriptorSetLayoutCreateInfo getCreateInfo(std::vector<VkDescriptorSetLayoutBinding> bindings) const;

public:
  VkDescriptorSetLayout handle;

  DescriptorSetLayout(Device* device, std::vector<VkDescriptorSetLayoutBinding> bindings);
  ~DescriptorSetLayout();
};