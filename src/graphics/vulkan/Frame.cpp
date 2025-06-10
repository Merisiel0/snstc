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
#include "VulkanHandler.h"

Frame::Frame() {
  commandPool = std::make_shared<CommandPool>(VulkanHandler::getDevice()->graphicsQueue->familyIndex);
  commandBuffer = std::make_shared<CommandBuffer>(*commandPool);

  swapchainSemaphore = std::make_shared<Semaphore>();
  renderSemaphore = std::make_shared<Semaphore>();
  renderFence = std::make_shared<Fence>(true);

  globalDescSet = DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_GLOBAL);
}

Frame::~Frame() {}