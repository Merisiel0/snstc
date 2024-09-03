#pragma once

#include "VulkanUtils.h"

class Device;

class Semaphore {
private:
  VkDevice* _devicePtr{ nullptr };

  VkSemaphoreCreateInfo getCreateInfo();

public:
  VkSemaphore handle{ VK_NULL_HANDLE };

  Semaphore() {}
  Semaphore(Device* device);
  ~Semaphore();

  VkSemaphoreSubmitInfo getSubmitInfo(VkPipelineStageFlags2 stageMask) const;
};