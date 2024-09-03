#pragma once

#include "VulkanUtils.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"

class Allocator {
private:
  VmaAllocatorCreateInfo& getCreateInfo(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);

public:
  VmaAllocator handle{ VK_NULL_HANDLE };

  Allocator() {}
  Allocator(Instance* instance, PhysicalDevice* physicalDevice, Device* device);
  ~Allocator();
};