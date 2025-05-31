#pragma once

#include "VulkanUtils.h"

class CommandPool {
private:
  VkCommandPoolCreateInfo getCreateInfo(uint32_t queueFamily) const;

public:
  VkCommandPool handle;

  CommandPool(uint32_t queueFamilyIndex);
  ~CommandPool();
};