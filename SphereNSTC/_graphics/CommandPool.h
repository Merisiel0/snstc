#pragma once

#include "VulkanUtils.h"

class Device;

class CommandPool {
private:
  VkDevice* _devicePtr{ nullptr };

  VkCommandPoolCreateInfo& getCreateInfo(VkDevice device, uint32_t queueFamily);

public:
  VkCommandPool handle{ VK_NULL_HANDLE };

  CommandPool() {}
  CommandPool(Device* device, uint32_t queueFamilyIndex);
  ~CommandPool();
};