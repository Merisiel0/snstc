#include "CommandBuffer.h"

#include "Device.h"
#include "CommandPool.h"
#include "Queue.h"
#include "Semaphore.h"
#include "Fence.h"
#include "Image.h"

VkCommandBufferAllocateInfo& CommandBuffer::getAllocateInfo(VkCommandPool commandPool) {
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  //info.pNext = nullptr;
  info.commandPool = commandPool;
  info.commandBufferCount = 1;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  return info;
}

VkCommandBufferBeginInfo& CommandBuffer::getBeginInfo(VkCommandBufferUsageFlags flags) const {
  VkCommandBufferBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  //info.pNext = nullptr;
  info.flags = flags;
  //info.pInheritanceInfo = nullptr;

  return info;
}

CommandBuffer::CommandBuffer(Device* device, CommandPool* commandPool) {
  VkCommandBufferAllocateInfo allocationInfo = getAllocateInfo(commandPool->handle);
  VK_CHECK(vkAllocateCommandBuffers(device->handle, &allocationInfo, &handle));
}

void CommandBuffer::reset() const {
  VK_CHECK(vkResetCommandBuffer(handle, 0));
}

void CommandBuffer::begin() const {
  VK_CHECK(vkBeginCommandBuffer(handle, &getBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)));
}

void CommandBuffer::beginRendering(Image* image) const {
  vkCmdBeginRendering(handle, &image->getRenderingInfo());
}

void CommandBuffer::endRendering() const {
  vkCmdEndRendering(handle);
}

void CommandBuffer::end() const {
  VK_CHECK(vkEndCommandBuffer(handle));
}

void CommandBuffer::submitToQueue(Queue* queue, Fence* fence, Semaphore* wait, Semaphore* signal) const {
  VkCommandBufferSubmitInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  //info.pNext = nullptr;
  info.commandBuffer = handle;
  //info.deviceMask = 0;

  VkSubmitInfo2 info2{};
  info2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  //info2.pNext = nullptr;
  //info2.flags = 0;
  
  if (wait != nullptr) {
    info2.waitSemaphoreInfoCount = 1;
    info2.pWaitSemaphoreInfos = &wait->getSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
  }
  
  if (signal != nullptr) {
    info2.signalSemaphoreInfoCount = 1;
    info2.pSignalSemaphoreInfos = &signal->getSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
  }

  info2.commandBufferInfoCount = 1;
  info2.pCommandBufferInfos = &info;

  VK_CHECK(vkQueueSubmit2(queue->handle, 1, &info2, fence->handle));
}
