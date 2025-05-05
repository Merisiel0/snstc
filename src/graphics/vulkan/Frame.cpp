#include "Frame.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "Fence.h"
#include "Queue.h"
#include "Semaphore.h"
#include "DescriptorManager.h"

Frame::Frame(std::shared_ptr<Device> device) {
  commandPool = std::make_shared<CommandPool>(device, device->graphicsQueue->familyIndex);
  commandBuffer = std::make_shared<CommandBuffer>(device, *commandPool);

  swapchainSemaphore = std::make_shared<Semaphore>(device);
  renderSemaphore = std::make_shared<Semaphore>(device);
  renderFence = std::make_shared<Fence>(device, true);

  globalDescSet = DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_GLOBAL);
  skyboxDescSet = DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_SKYBOX);
}

Frame::~Frame() {}