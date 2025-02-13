#pragma once

#include "VulkanUtils.h"

class Device;
class CommandPool;
class Queue;
class Semaphore;
class Fence;
class Image;
class GraphicsPipeline;
class DescriptorSet;
class Buffer;
struct PushConstants;
class IPipeline;

class CommandBuffer {
private:
  VkCommandBufferAllocateInfo getAllocateInfo(VkCommandPool commandPool) const;
  VkCommandBufferBeginInfo getBeginInfo(VkCommandBufferUsageFlags flags) const;
  VkCommandBufferSubmitInfo getSubmitInfo() const;

public:
  VkCommandBuffer handle;

  CommandBuffer(Device* device, CommandPool* commandPool);
  ~CommandBuffer() {};

  void reset() const;

  void begin() const;
  void beginRendering(Image* colorImage, Image* depthImage) const;
  void endRendering() const;
  void end() const;

  void bindPipeline(GraphicsPipeline* pipeline) const;
  void bindDescriptorSet(DescriptorSet* set, uint32_t setNb, IPipeline* pipeline) const;
  void setViewport(VkViewport* viewport) const;
  void setScissor(VkRect2D* scissor) const;
  void setLineWidth(float lineWidth) const;
  void setCullMode(VkCullModeFlags cullMode) const;
  void pushConstants(PushConstants constants, VkPipelineLayout layout, VkShaderStageFlags stage) const;
  void bindIndexBuffer(Buffer* buffer) const;
  void drawIndexed(uint32_t indexCount) const;
  void drawVertices(uint32_t vertexCount) const;

  void submitToQueue(Queue* queue, Fence* fence, Semaphore* wait, Semaphore* signal) const;
};