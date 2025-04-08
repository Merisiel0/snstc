#include "ImmediateSubmit.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "Queue.h"

ImmediateSubmit::ImmediateSubmit(std::shared_ptr<Device> device) {
  _commandPool = new CommandPool(device, device->graphicsQueue->familyIndex);
  _commandBuffer = new CommandBuffer(device, _commandPool);
  _fence = new Fence(device);
  _queue = device->graphicsQueue;
}

ImmediateSubmit::~ImmediateSubmit() {
  delete _commandPool;
  delete _commandBuffer;
  delete _fence;
}

void ImmediateSubmit::submit(std::function<void(CommandBuffer* cmd)>&& function) {
  _fence->reset();
  _commandBuffer->reset();

  _commandBuffer->begin();

  function(_commandBuffer);

  _commandBuffer->end();

  _commandBuffer->submitToQueue(_queue, _fence, nullptr, nullptr);

  _fence->wait();
}