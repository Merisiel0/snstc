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
  std::shared_ptr<CommandPool> commandPool;
  std::shared_ptr<CommandBuffer> commandBuffer;

  std::shared_ptr<Semaphore> swapchainSemaphore;
  std::shared_ptr<Semaphore> renderSemaphore;
  std::shared_ptr<Fence> renderFence;

  std::shared_ptr<DescriptorSet> camDescSet;
  std::shared_ptr<DescriptorSet> objDescSet;

  Frame(std::shared_ptr<Device> device, const DescriptorPool& pool,
    const DescriptorSetLayout& camDescSetLayout, const DescriptorSetLayout& objDescSetLayout);
  ~Frame();
};