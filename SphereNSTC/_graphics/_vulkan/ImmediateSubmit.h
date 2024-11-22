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
  CommandPool* _commandPool;
  CommandBuffer* _commandBuffer;
  Fence* _fence;
  Queue* _queue;

public:
  ImmediateSubmit(Device* device);
  ~ImmediateSubmit();

  void submit(std::function<void(CommandBuffer* cmd)>&& function);
};