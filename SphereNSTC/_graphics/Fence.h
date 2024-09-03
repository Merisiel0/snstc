#pragma once

#include "VulkanUtils.h"

class Device;

class Fence {
private:
  VkDevice* _devicePtr{ nullptr };

  VkFenceCreateInfo& getCreateInfo(bool signaled);

public:
  VkFence handle{ VK_NULL_HANDLE };

  Fence() {}
  Fence(Device* device, bool signaled = false);
  ~Fence();

  void reset();
  void wait();
};