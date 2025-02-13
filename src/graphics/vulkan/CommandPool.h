#pragma once

#include "VulkanUtils.h"

class Device;

class CommandPool {
private:
  VkDevice* _devicePtr;

  VkCommandPoolCreateInfo getCreateInfo(VkDevice device, uint32_t queueFamily) const;

public:
  VkCommandPool handle;

  CommandPool(Device* device, uint32_t queueFamilyIndex);
  ~CommandPool();
};