#pragma once

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

  Frame(Device* device, DescriptorPool* pool, DescriptorSetLayout* camDescSetLayout, DescriptorSetLayout* objDescSetLayout);
  ~Frame();
};