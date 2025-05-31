#pragma once

#include "VulkanUtils.h"

class Fence {
private:
  VkFenceCreateInfo getCreateInfo(bool signaled) const;

public:
  VkFence handle;

  Fence(bool signaled = false);
  ~Fence();

  void reset();
  void wait();
};