#pragma once

#include "VulkanUtils.h"

class Device;
class CommandBuffer;
class Frame;
class Semaphore;
class Swapchain;

class Queue {
private:
  VkPresentInfoKHR getPresentInfo(const Swapchain& swapchain, uint32_t imageIndex,
    const Semaphore& wait) const;

public:
  VkQueue handle;
  uint32_t familyIndex;

  Queue(Device* device, uint32_t familyIndex);
  ~Queue() {};

  void present(const Swapchain& swapchain, uint32_t imageIndex, const Semaphore& wait) const;
};