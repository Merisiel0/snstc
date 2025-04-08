#pragma once

#include "Device.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "VulkanUtils.h"

class Allocator {
private:
  VmaAllocatorCreateInfo getCreateInfo(VkInstance instance, VkPhysicalDevice physicalDevice,
    VkDevice device);

public:
  VmaAllocator handle;

  Allocator(std::shared_ptr<Instance>, std::shared_ptr<PhysicalDevice> physicalDevice,
    std::shared_ptr<Device> device);
  ~Allocator();
};