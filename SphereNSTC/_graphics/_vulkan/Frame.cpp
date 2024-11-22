#include "Frame.h"

#include "Device.h"
#include "Queue.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Fence.h"
#include "DescriptorSet.h"

Frame::Frame(Device* device, DescriptorPool* pool, DescriptorSetLayout* camDescSetLayout, DescriptorSetLayout* objDescSetLayout) {
  commandPool = new CommandPool(device, device->graphicsQueue->familyIndex);
  commandBuffer = new CommandBuffer(device, commandPool);

  swapchainSemaphore = new Semaphore(device);
  renderSemaphore = new Semaphore(device);
  renderFence = new Fence(device, true);

  camDescSet = new DescriptorSet(device, pool, camDescSetLayout);
  objDescSet = new DescriptorSet(device, pool, objDescSetLayout);
}

Frame::~Frame() {
  delete commandPool;
  delete commandBuffer;

  delete swapchainSemaphore;
  delete renderSemaphore;
  delete renderFence;

  delete camDescSet;
  delete objDescSet;
}