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

VkCommandBufferAllocateInfo CommandBuffer::getAllocateInfo(VkCommandPool commandPool) const {
  VkCommandBufferAllocateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  //info.pNext = nullptr;
  info.commandPool = commandPool;
  info.commandBufferCount = 1;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  return info;
}

VkCommandBufferBeginInfo CommandBuffer::getBeginInfo(VkCommandBufferUsageFlags flags) const {
  VkCommandBufferBeginInfo info {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  //info.pNext = nullptr;
  info.flags = flags;
  //info.pInheritanceInfo = nullptr;

  return info;
}

VkCommandBufferSubmitInfo CommandBuffer::getSubmitInfo() const {
  VkCommandBufferSubmitInfo info {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  //info.pNext = nullptr;
  info.commandBuffer = handle;
  //info.deviceMask = 0;

  return info;
}

CommandBuffer::CommandBuffer(std::shared_ptr<Device> device, CommandPool* commandPool) {
  VkCommandBufferAllocateInfo allocationInfo = getAllocateInfo(commandPool->handle);
  VK_CHECK(vkAllocateCommandBuffers(device->handle, &allocationInfo, &handle));
}

void CommandBuffer::reset() const { VK_CHECK(vkResetCommandBuffer(handle, 0)); }

void CommandBuffer::begin() const {
  VkCommandBufferBeginInfo beginInfo = getBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  VK_CHECK(vkBeginCommandBuffer(handle, &beginInfo));
}

void CommandBuffer::beginRendering(const Image& colorImage, const Image& depthImage) const {
  VkRenderingInfo renderingInfo = Image::getRenderingInfo(colorImage, depthImage);
  vkCmdBeginRendering(handle, &renderingInfo);
}

void CommandBuffer::endRendering() const { vkCmdEndRendering(handle); }

void CommandBuffer::end() const { VK_CHECK(vkEndCommandBuffer(handle)); }

void CommandBuffer::bindPipeline(const GraphicsPipeline& pipeline) const {
  vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
}

void CommandBuffer::bindDescriptorSet(DescriptorSet* set, uint32_t setNb,
  const IPipeline& pipeline) const {
  vkCmdBindDescriptorSets(handle, pipeline.type(), pipeline.layout(), setNb, 1, &set->handle, 0,
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

void CommandBuffer::drawIndexed(uint32_t indexCount) const {
  vkCmdDrawIndexed(handle, indexCount, 1, 0, 0, 0);
}

void CommandBuffer::drawVertices(uint32_t vertexCount) const {
  vkCmdDraw(handle, vertexCount, 1, 0, 0);
}

void CommandBuffer::submitToQueue(std::shared_ptr<Queue> queue, Fence* fence, Semaphore* wait,
  Semaphore* signal) const {
  VkSubmitInfo2 info2 {};
  info2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  //info2.pNext = nullptr;
  //info2.flags = 0;

  if(wait) {
    info2.waitSemaphoreInfoCount = 1;
    VkSemaphoreSubmitInfo waitSemaphoreSubmitInfo =
      wait->getSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
    info2.pWaitSemaphoreInfos = new VkSemaphoreSubmitInfo(waitSemaphoreSubmitInfo);
  }

  if(signal) {
    info2.signalSemaphoreInfoCount = 1;
    VkSemaphoreSubmitInfo signalSemaphoreSubmitInfo =
      signal->getSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    info2.pSignalSemaphoreInfos = new VkSemaphoreSubmitInfo(signalSemaphoreSubmitInfo);
  }

  info2.commandBufferInfoCount = 1;
  VkCommandBufferSubmitInfo submitInfo = getSubmitInfo();
  info2.pCommandBufferInfos = &submitInfo;

  VK_CHECK(vkQueueSubmit2(queue->handle, 1, &info2, fence->handle));
}
