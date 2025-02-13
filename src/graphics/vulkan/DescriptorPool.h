#pragma once

#include "VulkanUtils.h"

class Device;
class DescriptorSetLayout;

class DescriptorPool {
private:
  VkDevice* _devicePtr;

  VkDescriptorPoolCreateInfo getCreateInfo(std::vector<VkDescriptorPoolSize> poolSizes) const;

public:
  VkDescriptorPool handle;

  DescriptorPool(Device* device, std::vector<VkDescriptorPoolSize> poolSizes);
  ~DescriptorPool();

  void reset() const;
};