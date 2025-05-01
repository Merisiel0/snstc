#pragma once

#include <memory>
#include <vector>

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

  std::shared_ptr<DescriptorSet> globalDescSet;
  std::shared_ptr<DescriptorSet> skyboxDescSet;
  std::shared_ptr<DescriptorSet> materialDescSet;

  Frame(std::shared_ptr<Device> device, const DescriptorPool& pool,
    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts);
  ~Frame();
};