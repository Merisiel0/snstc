#pragma once

#include "VulkanUtils.h"

#include "Allocator.h"
#include "ImmediateSubmit.h"
#include "CommandBuffer.h"

class Device;
class Image;

class Buffer {
private:
  static inline VkDevice* _devicePtr;
  static inline VmaAllocator* _allocatorPtr;
  static inline ImmediateSubmit* _immediateSubmitPtr;

  VmaAllocation _allocation;
  VmaAllocationInfo _info;
  uint32_t _count;

  VkBufferCreateInfo getCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage) const;
  VmaAllocationCreateInfo getAllocationCreateInfo(VmaMemoryUsage usage) const;

  VkBufferImageCopy getBufferImageCopy(Image* image) const;

public:
  VkBuffer handle;
  VkDeviceAddress address;

  static void init(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit);

  Buffer(const VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

  template<typename T>
  Buffer(std::vector<T> data, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
    : Buffer(data.size() * sizeof(T), usage, memoryUsage) {
    _count = static_cast<uint32_t>(data.size());

    const size_t bufferSize = static_cast<size_t>(data.size() * sizeof(T));
    Buffer* staging = new Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    VK_CHECK(vmaMapMemory(*_allocatorPtr, staging->_allocation, &staging->_info.pMappedData));
    memcpy(staging->_info.pMappedData, data.data(), bufferSize);

    _immediateSubmitPtr->submit([&](CommandBuffer* cmd) {
      VkBufferCopy copy{};
      //copy.dstOffset = 0;
      //copy.srcOffset = 0;
      copy.size = bufferSize;

      vkCmdCopyBuffer(cmd->handle, staging->handle, handle, 1, &copy);
      });

    vmaUnmapMemory(*_allocatorPtr, staging->_allocation);
    delete staging;
  }

  ~Buffer();

  uint32_t count() const { return _count; }
  VmaAllocation allocation() const { return _allocation; }
  VkBufferDeviceAddressInfo getDeviceAddressInfo() const;
  void copyToImage(CommandBuffer* commandBuffer, Image* image) const;

  template<typename T>
  void update(CommandBuffer* commandBuffer, std::vector<T> data) {
    vkCmdUpdateBuffer(commandBuffer->handle, handle, 0, data.size() * sizeof(T), data.data());
    _count = (uint32_t)data.size();
  }
};