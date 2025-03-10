#include "Frame.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Fence.h"
#include "Queue.h"
#include "Semaphore.h"

Frame::Frame(std::shared_ptr<Device> device,
             const DescriptorPool& pool,
             const DescriptorSetLayout& camDescSetLayout,
             const DescriptorSetLayout& objDescSetLayout) {
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
}