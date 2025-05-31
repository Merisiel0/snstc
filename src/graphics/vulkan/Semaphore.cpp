#include "Semaphore.h"

#include "Device.h"
#include "VulkanHandler.h"

VkSemaphoreCreateInfo Semaphore::getCreateInfo() const {
  VkSemaphoreCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;

  return info;
}

Semaphore::Semaphore() {
  VkSemaphoreCreateInfo createInfo = getCreateInfo();
  VK_CHECK(vkCreateSemaphore(VulkanHandler::getDevice()->handle, &createInfo, nullptr, &handle));
}

Semaphore::~Semaphore() { vkDestroySemaphore(VulkanHandler::getDevice()->handle, handle, nullptr); }

VkSemaphoreSubmitInfo Semaphore::getSubmitInfo(VkPipelineStageFlags2 stageMask) const {
  VkSemaphoreSubmitInfo info;
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  info.pNext = nullptr;
  info.semaphore = handle;
  info.value = 1;
  info.stageMask = stageMask;
  info.deviceIndex = 0;

  return info;
}
