#include "Frame.h"

#include "Device.h"
#include "Queue.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Fence.h"

Frame::Frame(Device* device) {
  commandPool = new CommandPool(device, device->graphicsQueue->familyIndex);
  commandBuffer = new CommandBuffer(device, commandPool);

  swapchainSemaphore = new Semaphore(device);
  renderSemaphore = new Semaphore(device);
  renderFence = new Fence(device);
}

Frame::~Frame() {
  delete commandPool;
  delete commandBuffer;

  delete swapchainSemaphore;
  delete renderSemaphore;
  delete renderFence;
}