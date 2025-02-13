#pragma once

#include "VulkanUtils.h"

class Device;

class Semaphore {
private:
  VkDevice* _devicePtr;

  VkSemaphoreCreateInfo getCreateInfo() const;

public:
  VkSemaphore handle;

  Semaphore(Device* device);
  ~Semaphore();

  VkSemaphoreSubmitInfo getSubmitInfo(VkPipelineStageFlags2 stageMask) const;
};