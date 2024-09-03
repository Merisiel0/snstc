#include "Allocator.h"

VmaAllocatorCreateInfo& Allocator::getCreateInfo(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device) {
  VmaAllocatorCreateInfo info{};
  info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  info.physicalDevice = physicalDevice;
  info.device = device;
  info.instance = instance;
  info.vulkanApiVersion = VK_API_VERSION_1_3;

  return info;
}

Allocator::Allocator(Instance* instance, PhysicalDevice* physicalDevice, Device* device) {
  VmaAllocatorCreateInfo createInfo = getCreateInfo(instance->handle, physicalDevice->handle, device->handle);
  VK_CHECK(vmaCreateAllocator(&createInfo, &handle));
}

Allocator::~Allocator() {
  vmaDestroyAllocator(handle);
}