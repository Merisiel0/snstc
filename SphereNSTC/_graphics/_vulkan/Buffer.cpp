#include "Buffer.h"

#include "Device.h"
#include "Allocator.h"
#include "Image.h"
#include "CommandBuffer.h"

VkBufferCreateInfo Buffer::getCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage) const {
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;
  info.size = size;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //info.queueFamilyIndexCount = 0;
  //info.pQueueFamilyIndices = nullptr;

  return info;
}

VmaAllocationCreateInfo Buffer::getAllocationCreateInfo(VmaMemoryUsage usage) const {
  VmaAllocationCreateInfo info{};
  info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  info.usage = usage;
  //info.requiredFlags = 0;
  //info.preferredFlags = 0;
  //info.memoryTypeBits = 0;
  //info.pool = VK_NULL_HANDLE;
  //info.pUserData = nullptr;
  //info.priority = 0;

  return info;
}

VkBufferImageCopy Buffer::getBufferImageCopy(Image* image) const {
  VkBufferImageCopy copyRegion{};
  //copyRegion.bufferOffset = 0;
  //copyRegion.bufferRowLength = 0;
  //copyRegion.bufferImageHeight = 0;
  copyRegion.imageSubresource = image->getSubresourceLayers();
  copyRegion.imageExtent.width = image->extent().width;
  copyRegion.imageExtent.height = image->extent().height;
  copyRegion.imageExtent.depth = 1;

  return copyRegion;
}

void Buffer::init(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit) {
  _devicePtr = &device->handle;
  _allocatorPtr = &allocator->handle;
  _immediateSubmitPtr = immediateSubmit;
}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
  usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  VkBufferCreateInfo createInfo = getCreateInfo(size, usage);
  VmaAllocationCreateInfo vmaInfo = getAllocationCreateInfo(memoryUsage);
  VK_CHECK(vmaCreateBuffer(*_allocatorPtr, &createInfo, &vmaInfo, &handle, &_allocation, &_info));

  VkBufferDeviceAddressInfo deviceAddressInfo = getDeviceAddressInfo();
  address = vkGetBufferDeviceAddress(*_devicePtr, &deviceAddressInfo);
}

Buffer::~Buffer() {
  vmaDestroyBuffer(*_allocatorPtr, handle, _allocation);
}

VkBufferDeviceAddressInfo Buffer::getDeviceAddressInfo() const {
  VkBufferDeviceAddressInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  //info->pNext = nullptr;
  info.buffer = handle;

  return info;
}

void Buffer::copyToImage(CommandBuffer* commandBuffer, Image* image) const {
  VkBufferImageCopy copyRegion = getBufferImageCopy(image);
  vkCmdCopyBufferToImage(commandBuffer->handle, handle, image->handle, image->layout(), 1, &copyRegion);
}
