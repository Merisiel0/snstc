#pragma once

#include "VulkanUtils.h"

class Device;
class CommandPool;
class Queue;
class Semaphore;
class Fence;
class Image;
class GraphicsPipeline;
class Buffer;
struct PushConstants;

class CommandBuffer {
private:
  VkCommandBufferAllocateInfo getAllocateInfo(VkCommandPool commandPool);
  VkCommandBufferBeginInfo getBeginInfo(VkCommandBufferUsageFlags flags) const;

public:
  VkCommandBuffer handle{ VK_NULL_HANDLE };

  CommandBuffer() {}
  CommandBuffer(Device* device, CommandPool* commandPool);
  ~CommandBuffer() {};

  void reset() const;

  void begin() const;
  void beginRendering(Image* image) const;
  void endRendering() const;
  void end() const;

  void bindPipeline(GraphicsPipeline* pipeline) const;
  void setViewport(VkViewport* viewport) const;
  void setScissor(VkRect2D* scissor) const;
  void setLineWidth(float lineWidth) const;
  void setCullMode(VkCullModeFlags cullMode) const;
  void pushConstants(PushConstants constants, VkPipelineLayout layout, VkShaderStageFlags stage) const;
  void bindIndexBuffer(Buffer* buffer) const;
  void drawIndexed(uint32_t indexCount) const;

  void submitToQueue(Queue* queue, Fence* fence, Semaphore* wait = nullptr, Semaphore* signal = nullptr) const;
};