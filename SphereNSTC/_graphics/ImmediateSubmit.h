#pragma once

#include "VulkanUtils.h"
#include <functional>

class Device;
class CommandPool;
class CommandBuffer;
class Fence;
class Queue;

class ImmediateSubmit {
private:
  CommandPool* _commandPool{ nullptr };
  CommandBuffer* _commandBuffer{ nullptr };
  Fence* _fence{ nullptr };
  Queue* _queue{ nullptr };

public:
  ImmediateSubmit() {};
  ImmediateSubmit(Device* device);
  ~ImmediateSubmit();

  void submit(std::function<void(VkCommandBuffer cmd)>&& function);
};