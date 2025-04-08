#pragma once

#include "VulkanUtils.h"

class Device;

class Fence {
private:
  std::shared_ptr<Device> _device;

  VkFenceCreateInfo getCreateInfo(bool signaled) const;

public:
  VkFence handle;

  Fence(std::shared_ptr<Device> device, bool signaled = false);
  ~Fence();

  void reset();
  void wait();
};