#include "Allocator.h"

#include "VulkanUtils.h"

VmaAllocatorCreateInfo Allocator::getCreateInfo(VkInstance instance,
  VkPhysicalDevice physicalDevice, VkDevice device) {
  VmaAllocatorCreateInfo info {};
  info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  info.physicalDevice = physicalDevice;
  info.device = device;
  info.preferredLargeHeapBlockSize = 0;
  info.pAllocationCallbacks = nullptr;
  info.pDeviceMemoryCallbacks = nullptr;
  info.pHeapSizeLimit = nullptr;
  info.pVulkanFunctions = nullptr;
  info.instance = instance;
  info.vulkanApiVersion = VK_API_VERSION_1_3;
  info.pTypeExternalMemoryHandleTypes = nullptr;

  return info;
}

Allocator::Allocator(std::shared_ptr<Instance> instance,
  std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<Device> device) {
  VmaAllocatorCreateInfo createInfo =
    getCreateInfo(instance->handle, physicalDevice->handle, device->handle);
  VK_CHECK(vmaCreateAllocator(&createInfo, &handle));
}

Allocator::~Allocator() { vmaDestroyAllocator(handle); }