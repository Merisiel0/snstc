#pragma once

#include "VulkanUtils.h"

class Device;
class CommandBuffer;
class Frame;
class Semaphore;
class Swapchain;

class Queue {
public:
  VkQueue handle{ VK_NULL_HANDLE };
  uint32_t familyIndex{ UINT32_MAX };

  Queue() {}
  Queue(Device* device, uint32_t familyIndex);
  ~Queue() {};

  void present(Swapchain* swapchain, uint32_t imageIndex, Semaphore* wait) const;
};