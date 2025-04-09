#include "ImmediateSubmit.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "Queue.h"

ImmediateSubmit::ImmediateSubmit(std::shared_ptr<Device> device) {
  _commandPool = std::make_shared<CommandPool>(device, device->graphicsQueue->familyIndex);
  _commandBuffer = std::make_shared<CommandBuffer>(device, *_commandPool);
  _fence = std::make_shared<Fence>(device);
  _queue = device->graphicsQueue;
}

ImmediateSubmit::~ImmediateSubmit() {}

void ImmediateSubmit::submit(std::function<void(std::shared_ptr<CommandBuffer> cmd)>&& function) {
  _fence->reset();
  _commandBuffer->reset();

  _commandBuffer->begin();

  function(_commandBuffer);

  _commandBuffer->end();

  _commandBuffer->submitToQueue(*_queue, *_fence, nullptr, nullptr);

  _fence->wait();
}