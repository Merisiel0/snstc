#pragma once

#include "VulkanUtils.h"

class Device;

class CommandPool {
private:
  std::shared_ptr<Device> _device;

  VkCommandPoolCreateInfo getCreateInfo(VkDevice device, uint32_t queueFamily) const;

public:
  VkCommandPool handle;

  CommandPool(std::shared_ptr<Device> device, uint32_t queueFamilyIndex);
  ~CommandPool();
};