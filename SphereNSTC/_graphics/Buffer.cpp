#include "Buffer.h"

#include "Device.h"
#include "Allocator.h"

Buffer::Buffer(Device* device, Allocator* allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
  _allocatorPtr = &allocator->handle;

  usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  VkBufferCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  //createInfo.pNext = nullptr;
  //createInfo.flags = 0;
  createInfo.size = size;
  createInfo.usage = usage;
  createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //createInfo.queueFamilyIndexCount = 0;
  //createInfo.pQueueFamilyIndices = nullptr;

  VmaAllocationCreateInfo vmaInfo{};
  vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  vmaInfo.usage = memoryUsage;
  //vmaInfo.requiredFlags = 0;
  //vmaInfo.preferredFlags = 0;
  //vmaInfo.memoryTypeBits = 0;
  //vmaInfo.pool = VK_NULL_HANDLE;
  //vmaInfo.pUserData = nullptr;
  //vmaInfo.priority = 0;

  VK_CHECK(vmaCreateBuffer(allocator->handle, &createInfo, &vmaInfo, &handle, &_allocation, &_info));

  VkBufferDeviceAddressInfo deviceAdressInfo = getDeviceAddressInfo();
  address = vkGetBufferDeviceAddress(device->handle, &deviceAdressInfo);
}

Buffer::~Buffer() {
  vmaDestroyBuffer(*_allocatorPtr, handle, _allocation);
}

VkBufferDeviceAddressInfo Buffer::getDeviceAddressInfo() const {
  VkBufferDeviceAddressInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  //info.pNext = nullptr;
  info.buffer = handle;

  return info;
}