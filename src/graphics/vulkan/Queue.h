#pragma once

#include "VulkanUtils.h"

class Device;
class CommandBuffer;
class Frame;
class Semaphore;
class Swapchain;

class Queue {
private:
  VkPresentInfoKHR getPresentInfo(Swapchain* swapchain, uint32_t imageIndex, Semaphore* wait) const;

public:
  VkQueue handle;
  uint32_t familyIndex;

  Queue(Device* device, uint32_t familyIndex);
  ~Queue() {};

  void present(Swapchain* swapchain, uint32_t imageIndex, Semaphore* wait) const;
};