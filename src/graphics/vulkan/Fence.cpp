#include "Fence.h"

#include "Device.h"

VkFenceCreateInfo Fence::getCreateInfo(bool signaled) const {
  VkFenceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  //info.pNext = nullptr;
  info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  return info;
}

Fence::Fence(Device* device, bool signaled) {
  this->_devicePtr = &device->handle;

  VkFenceCreateInfo createInfo = getCreateInfo(signaled);
  VK_CHECK(vkCreateFence(device->handle, &createInfo, nullptr, &handle));
}

Fence::~Fence() {
  vkDestroyFence(*_devicePtr, handle, nullptr);
}

void Fence::reset() {
  VK_CHECK(vkResetFences(*_devicePtr, 1, &handle));
}

void Fence::wait() {
  VK_CHECK(vkWaitForFences(*_devicePtr, 1, &handle, true, 9999999999));
}