#pragma once

#include "VulkanUtils.h"

class Device;

class Semaphore {
  private:
  std::shared_ptr<Device> _device;

  VkSemaphoreCreateInfo getCreateInfo() const;

  public:
  VkSemaphore handle;

  Semaphore(std::shared_ptr<Device> device);
  ~Semaphore();

  VkSemaphoreSubmitInfo getSubmitInfo(VkPipelineStageFlags2 stageMask) const;
};