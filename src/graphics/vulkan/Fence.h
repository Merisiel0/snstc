#pragma once

#include "VulkanUtils.h"

class Device;

class Fence {
private:
  VkDevice* _devicePtr{ nullptr };

  VkFenceCreateInfo getCreateInfo(bool signaled) const;

public:
  VkFence handle;

  Fence(Device* device, bool signaled = false);
  ~Fence();

  void reset();
  void wait();
};