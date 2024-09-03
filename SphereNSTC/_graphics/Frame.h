#pragma once

class Device;
class CommandPool;
class CommandBuffer;
class Semaphore;
class Fence;

class Frame {
public:
  CommandPool* commandPool{ nullptr };
  CommandBuffer* commandBuffer{ nullptr };

  Semaphore* swapchainSemaphore{ nullptr };
  Semaphore* renderSemaphore{ nullptr };
  Fence* renderFence{ nullptr };

  Frame() {}
  Frame(Device* device);
  ~Frame();
};