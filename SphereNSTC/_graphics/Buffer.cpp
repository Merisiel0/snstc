#include "Buffer.h"

#include "Device.h"
#include "Allocator.h"
#include "Image.h"
#include "CommandBuffer.h"

void Buffer::init(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit) {
  _devicePtr = &device->handle;
  _allocatorPtr = &allocator->handle;
  _immediateSubmitPtr = immediateSubmit;
}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
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

  VK_CHECK(vmaCreateBuffer(*_allocatorPtr, &createInfo, &vmaInfo, &handle, &_allocation, &_info));

  VkBufferDeviceAddressInfo deviceAdressInfo = getDeviceAddressInfo();
  address = vkGetBufferDeviceAddress(*_devicePtr, &deviceAdressInfo);
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

void Buffer::copyToImage(CommandBuffer* commandBuffer, Image* image) const {
  VkBufferImageCopy copyRegion = {};
  //copyRegion.bufferOffset = 0;
  //copyRegion.bufferRowLength = 0;
  //copyRegion.bufferImageHeight = 0;

  copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  //copyRegion.imageSubresource.mipLevel = 0;
  //copyRegion.imageSubresource.baseArrayLayer = 0;
  copyRegion.imageSubresource.layerCount = 1;
  copyRegion.imageExtent.width = image->extent.width;
  copyRegion.imageExtent.height = image->extent.height;
  copyRegion.imageExtent.depth = 1;

  //copy the buffer into the image
  vkCmdCopyBufferToImage(commandBuffer->handle, handle, image->handle, image->layout(), 1, &copyRegion);
}
