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

  CommandBuffer() : handle {VK_NULL_HANDLE} {}
  CommandBuffer(std::shared_ptr<Device>, const CommandPool& commandPool);
  ~CommandBuffer() {};

  void reset() const;

  void begin() const;
  void beginRendering(const Image& colorImage, const Image& depthImage) const;
  void endRendering() const;
  void end() const;

  void bindPipeline(const GraphicsPipeline& pipeline) const;
  void bindDescriptorSet(const DescriptorSet& set, uint32_t setNb, const IPipeline& pipeline) const;
  void setViewport(VkViewport* viewport) const;
  void setScissor(VkRect2D* scissor) const;
  void setLineWidth(float lineWidth) const;
  void setCullMode(VkCullModeFlags cullMode) const;
  void pushConstants(PushConstants constants, VkPipelineLayout layout,
    VkShaderStageFlags stage) const;
  void bindIndexBuffer(Buffer* buffer) const;
  void drawIndexed(uint32_t indexCount) const;
  void drawVertices(uint32_t vertexCount) const;

  void submitToQueue(const Queue& queue, const Fence& fence, std::shared_ptr<Semaphore> wait,
    std::shared_ptr<Semaphore> signal) const;
};