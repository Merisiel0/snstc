#include "CommandPool.h"

#include "Device.h"

VkCommandPoolCreateInfo CommandPool::getCreateInfo(VkDevice device, uint32_t queueFamily) const {
  VkCommandPoolCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  //info.pNext = nullptr;
  info.queueFamilyIndex = queueFamily;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  return info;
}

CommandPool::CommandPool(std::shared_ptr<Device> device, uint32_t queueFamilyIndex) {
  _device = device;

  VkCommandPoolCreateInfo createInfo = getCreateInfo(device->handle, queueFamilyIndex);
  VK_CHECK(vkCreateCommandPool(device->handle, &createInfo, nullptr, &handle));
}

CommandPool::~CommandPool() { vkDestroyCommandPool(_device->handle, handle, nullptr); }