#pragma once

#include "VulkanUtils.h"

class Device;
class DescriptorSetLayout;

class DescriptorPool {
  private:
  std::shared_ptr<Device> _device;

  VkDescriptorPoolCreateInfo getCreateInfo(std::vector<VkDescriptorPoolSize> poolSizes) const;

  public:
  VkDescriptorPool handle;

  DescriptorPool(std::shared_ptr<Device>, std::vector<VkDescriptorPoolSize> poolSizes);
  ~DescriptorPool();

  void reset() const;
};