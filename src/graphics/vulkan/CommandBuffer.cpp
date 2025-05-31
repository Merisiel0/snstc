#include "CommandBuffer.h"

#include "Buffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Fence.h"
#include "GraphicsPipeline.h"
#include "Image.h"
#include "Queue.h"
#include "Semaphore.h"
#include "VulkanHandler.h"

VkCommandBufferAllocateInfo CommandBuffer::getAllocateInfo(VkCommandPool commandPool) const {
  VkCommandBufferAllocateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;
  info.commandPool = commandPool;
  info.commandBufferCount = 1;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  return info;
}

VkCommandBufferBeginInfo CommandBuffer::getBeginInfo(VkCommandBufferUsageFlags flags) const {
  VkCommandBufferBeginInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  info.pInheritanceInfo = nullptr;

  return info;
}

VkCommandBufferSubmitInfo CommandBuffer::getSubmitInfo() const {
  VkCommandBufferSubmitInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  info.pNext = nullptr;
  info.commandBuffer = handle;
  info.deviceMask = 0;

  return info;
}

CommandBuffer::CommandBuffer(const CommandPool& commandPool) {
  VkCommandBufferAllocateInfo allocationInfo = getAllocateInfo(commandPool.handle);
  VK_CHECK(vkAllocateCommandBuffers(VulkanHandler::getDevice()->handle, &allocationInfo, &handle));
}

void CommandBuffer::reset() const { VK_CHECK(vkResetCommandBuffer(handle, 0)); }

void CommandBuffer::begin() const {
  VkCommandBufferBeginInfo beginInfo = getBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  VK_CHECK(vkBeginCommandBuffer(handle, &beginInfo));
}

void CommandBuffer::beginRendering(const Image& colorImage, const Image& depthImage) const {
  RenderingInfoData renderingInfo = Image::getRenderingInfo(colorImage, depthImage);
  vkCmdBeginRendering(handle, &renderingInfo.info);
}

void CommandBuffer::endRendering() const { vkCmdEndRendering(handle); }

void CommandBuffer::end() const { VK_CHECK(vkEndCommandBuffer(handle)); }

void CommandBuffer::bindPipeline(const GraphicsPipeline& pipeline) const {
  vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
}

void CommandBuffer::bindDescriptorSet(const DescriptorSet& set, uint32_t setNb,
  const IPipeline& pipeline) const {
  VkDescriptorSet setHandle = set.getHandle();
  vkCmdBindDescriptorSets(handle, pipeline.type(), pipeline.layout(), setNb, 1, &setHandle, 0,
    nullptr);
}

void CommandBuffer::setViewport(VkViewport* viewport) const {
  vkCmdSetViewportWithCount(handle, 1, viewport);
}

void CommandBuffer::setScissor(VkRect2D* scissor) const {
  vkCmdSetScissorWithCount(handle, 1, scissor);
}

void CommandBuffer::setLineWidth(float lineWidth) const { vkCmdSetLineWidth(handle, lineWidth); }

void CommandBuffer::setCullMode(VkCullModeFlags cullMode) const {
  vkCmdSetCullMode(handle, cullMode);
}

void CommandBuffer::pushConstants(PushConstants constants, VkPipelineLayout layout,
  VkShaderStageFlags stage) const {
  vkCmdPushConstants(handle, layout, stage, 0, sizeof(PushConstants), &constants);
}

void CommandBuffer::bindIndexBuffer(Buffer* buffer) const {
  vkCmdBindIndexBuffer(handle, buffer->handle, 0, VK_INDEX_TYPE_UINT32);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount) const {
  vkCmdDrawIndexed(handle, indexCount, instanceCount, 0, 0, 0);
}

void CommandBuffer::drawVertices(uint32_t vertexCount) const {
  vkCmdDraw(handle, vertexCount, 1, 0, 0);
}

void CommandBuffer::submitToQueue(const Queue& queue, const Fence& fence,
  std::shared_ptr<Semaphore> wait, std::shared_ptr<Semaphore> signal) const {
  SubmitInfo2Data data;

  if(wait) {
    data.waitSemaphoreInfos.push_back(
      wait->getSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR));
  }

  data.commandBufferSubmitInfos.push_back(getSubmitInfo());

  if(signal) {
    data.signalSemaphoreInfos.push_back(
      signal->getSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT));
  }

  data.info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  data.info.pNext = nullptr;
  data.info.flags = 0;
  data.info.waitSemaphoreInfoCount = (uint32_t) data.waitSemaphoreInfos.size();
  data.info.pWaitSemaphoreInfos = data.waitSemaphoreInfos.data();
  data.info.signalSemaphoreInfoCount = (uint32_t) data.signalSemaphoreInfos.size();
  data.info.pSignalSemaphoreInfos = data.signalSemaphoreInfos.data();
  data.info.commandBufferInfoCount = (uint32_t) data.commandBufferSubmitInfos.size();
  data.info.pCommandBufferInfos = data.commandBufferSubmitInfos.data();

  VK_CHECK(vkQueueSubmit2(queue.handle, 1, &data.info, fence.handle));
}
