#include "Buffer.h"

#include "Allocator.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "Image.h"

VkBufferCreateInfo Buffer::getCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage) const {
  VkBufferCreateInfo info {};
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
  VmaAllocationCreateInfo info {};
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

VkBufferImageCopy Buffer::getBufferImageCopy(const Image& image) const {
  VkBufferImageCopy copyRegion {};
  //copyRegion.bufferOffset = 0;
  //copyRegion.bufferRowLength = 0;
  //copyRegion.bufferImageHeight = 0;
  copyRegion.imageSubresource = image.getSubresourceLayers();
  copyRegion.imageExtent.width = image.extent().width;
  copyRegion.imageExtent.height = image.extent().height;
  copyRegion.imageExtent.depth = 1;

  return copyRegion;
}

void Buffer::init(std::weak_ptr<Device> device, std::weak_ptr<Allocator> allocator,
  std::weak_ptr<ImmediateSubmit> immediateSubmit) {
  _device = device;
  _allocator = allocator;
  _immediateSubmit = immediateSubmit;
}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
  std::shared_ptr<Device> deviceSptr = getShared(_device);
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);

  usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  VkBufferCreateInfo createInfo = getCreateInfo(size, usage);
  VmaAllocationCreateInfo vmaInfo = getAllocationCreateInfo(memoryUsage);
  VK_CHECK(
    vmaCreateBuffer(allocatorSptr->handle, &createInfo, &vmaInfo, &handle, &_allocation, &_info));

  VkBufferDeviceAddressInfo deviceAddressInfo = getDeviceAddressInfo();
  address = vkGetBufferDeviceAddress(deviceSptr->handle, &deviceAddressInfo);
}

Buffer::~Buffer() {
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);

  vmaDestroyBuffer(allocatorSptr->handle, handle, _allocation);
}

VkBufferDeviceAddressInfo Buffer::getDeviceAddressInfo() const {
  VkBufferDeviceAddressInfo info {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  //info->pNext = nullptr;
  info.buffer = handle;

  return info;
}

void Buffer::copyToImage(std::shared_ptr<CommandBuffer> commandBuffer, const Image& image) const {
  VkBufferImageCopy copyRegion = getBufferImageCopy(image);
  vkCmdCopyBufferToImage(commandBuffer->handle, handle, image.handle, image.layout(), 1,
    &copyRegion);
}
