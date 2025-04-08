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
  std::shared_ptr<Queue> _queue;

public:
  ImmediateSubmit(std::shared_ptr<Device> device);
  ~ImmediateSubmit();

  void submit(std::function<void(CommandBuffer* cmd)>&& function);
};