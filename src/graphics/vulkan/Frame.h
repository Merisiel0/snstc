#pragma once

#include <memory>

class Device;
class CommandPool;
class CommandBuffer;
class Semaphore;
class Fence;
class DescriptorPool;
class DescriptorSetLayout;
class DescriptorSet;

class Frame {
 public:
  CommandPool* commandPool;
  CommandBuffer* commandBuffer;

  Semaphore* swapchainSemaphore;
  Semaphore* renderSemaphore;
  Fence* renderFence;

  DescriptorSet* camDescSet;
  DescriptorSet* objDescSet;

  Frame(std::shared_ptr<Device> device, const DescriptorPool& pool,
        const DescriptorSetLayout& camDescSetLayout,
        const DescriptorSetLayout& objDescSetLayout);
  ~Frame();
};