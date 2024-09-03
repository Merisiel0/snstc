#pragma once

#include "VulkanUtils.h"

class Device;
class CommandPool;
class Queue;
class Semaphore;
class Fence;
class Image;

class CommandBuffer {
private:
  VkCommandBufferAllocateInfo& getAllocateInfo(VkCommandPool commandPool);
  VkCommandBufferBeginInfo& getBeginInfo(VkCommandBufferUsageFlags flags) const;

public:
  VkCommandBuffer handle{ VK_NULL_HANDLE };

  CommandBuffer() {}
  CommandBuffer(Device* device, CommandPool* commandPool);
  ~CommandBuffer() {};

  void reset() const;
  void begin() const;
  void beginRendering(Image* image) const;
  void endRendering() const;
  void end() const;
  void submitToQueue(Queue* queue, Fence* fence, Semaphore* wait = nullptr, Semaphore* signal = nullptr) const;
};