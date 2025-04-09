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
  std::shared_ptr<CommandPool> _commandPool;
  std::shared_ptr<CommandBuffer> _commandBuffer;
  std::shared_ptr<Fence> _fence;
  std::shared_ptr<Queue> _queue;

public:
  ImmediateSubmit(std::shared_ptr<Device> device);
  ~ImmediateSubmit();

  void submit(std::function<void(std::shared_ptr<CommandBuffer> cmd)>&& function);
};