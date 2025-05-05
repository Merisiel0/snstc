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

  std::shared_ptr<DescriptorSet> globalDescSet; // TODO: move to World class
  std::shared_ptr<DescriptorSet> skyboxDescSet; // TODO: move to Skybox component (also remove material from skybox, make it only an image)

  Frame(std::shared_ptr<Device> device);
  ~Frame();
};