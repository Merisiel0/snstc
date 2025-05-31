#include "CommandPool.h"

#include "Device.h"
#include "VulkanHandler.h"

VkCommandPoolCreateInfo CommandPool::getCreateInfo(uint32_t queueFamily) const {
  VkCommandPoolCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.queueFamilyIndex = queueFamily;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  return info;
}

CommandPool::CommandPool(uint32_t queueFamilyIndex) {
  VkCommandPoolCreateInfo createInfo = getCreateInfo(queueFamilyIndex);
  VK_CHECK(vkCreateCommandPool(VulkanHandler::getDevice()->handle, &createInfo, nullptr, &handle));
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(VulkanHandler::getDevice()->handle, handle, nullptr);
}