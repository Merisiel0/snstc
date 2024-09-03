#pragma once

#include "VulkanUtils.h"

#include "Allocator.h"
#include "ImmediateSubmit.h"

class Device;

class Buffer {
private:
  VmaAllocator* _allocatorPtr{ nullptr };
  VmaAllocation _allocation{ VK_NULL_HANDLE };
  VmaAllocationInfo _info{};

public:
  VkBuffer handle{ VK_NULL_HANDLE };
  VkDeviceAddress address{};

  Buffer() {}

  Buffer(Device* device, Allocator* allocator, const VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

  template<typename T>
  Buffer(Device* device, Allocator* allocator, ImmediateSubmit* immmediateSubmit, std::vector<T> data, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
    : Buffer(device, allocator, data.size() * sizeof(T), usage, memoryUsage) {
    const size_t bufferSize = data.size() * sizeof(T);
    Buffer* staging = new Buffer(device, allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    VK_CHECK(vmaMapMemory(allocator->handle, _allocation, &staging->_info.pMappedData));
    memcpy(staging->_info.pMappedData, data.data(), bufferSize);

    immmediateSubmit->submit([&](VkCommandBuffer cmd) {
      VkBufferCopy copy{};
      //copy.dstOffset = 0;
      //copy.srcOffset = 0;
      copy.size = bufferSize;

      vkCmdCopyBuffer(cmd, staging->handle, handle, 1, &copy);
      });

    delete staging;
  }

  ~Buffer();

  VkBufferDeviceAddressInfo getDeviceAddressInfo() const;
};