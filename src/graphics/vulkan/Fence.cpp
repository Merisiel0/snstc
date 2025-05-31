#include "Fence.h"

#include "Device.h"
#include "VulkanHandler.h"

VkFenceCreateInfo Fence::getCreateInfo(bool signaled) const {
  VkFenceCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  return info;
}

Fence::Fence(bool signaled) {
  VkFenceCreateInfo createInfo = getCreateInfo(signaled);
  VK_CHECK(vkCreateFence(VulkanHandler::getDevice()->handle, &createInfo, nullptr, &handle));
}

Fence::~Fence() { vkDestroyFence(VulkanHandler::getDevice()->handle, handle, nullptr); }

void Fence::reset() { VK_CHECK(vkResetFences(VulkanHandler::getDevice()->handle, 1, &handle)); }

void Fence::wait() { VK_CHECK(vkWaitForFences(VulkanHandler::getDevice()->handle, 1, &handle, true, 9999999999)); }