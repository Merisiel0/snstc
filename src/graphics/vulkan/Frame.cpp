#include "Frame.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Fence.h"
#include "Queue.h"
#include "Semaphore.h"

Frame::Frame(std::shared_ptr<Device> device, const DescriptorPool& pool,
  const DescriptorSetLayout& camDescSetLayout, const DescriptorSetLayout& objDescSetLayout) {
  commandPool = std::make_shared<CommandPool>(device, device->graphicsQueue->familyIndex);
  commandBuffer = std::make_shared<CommandBuffer>(device, *commandPool);

  swapchainSemaphore = std::make_shared<Semaphore>(device);
  renderSemaphore = std::make_shared<Semaphore>(device);
  renderFence = std::make_shared<Fence>(device, true);

  camDescSet = std::make_shared<DescriptorSet>(device, pool, camDescSetLayout);
  objDescSet = std::make_shared<DescriptorSet>(device, pool, objDescSetLayout);
}

Frame::~Frame() {}