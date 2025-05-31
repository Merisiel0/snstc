#pragma once

#include "VulkanUtils.h"

class Semaphore {
private:
  VkSemaphoreCreateInfo getCreateInfo() const;

public:
  VkSemaphore handle;

  Semaphore();
  ~Semaphore();

  VkSemaphoreSubmitInfo getSubmitInfo(VkPipelineStageFlags2 stageMask) const;
};