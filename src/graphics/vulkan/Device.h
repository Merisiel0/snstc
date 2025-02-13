#pragma once

#include "VulkanUtils.h"

class PhysicalDevice;
class Queue;

class Device {
private:
  PhysicalDevice* _physicalDevicePtr{ nullptr };

  VkDeviceCreateInfo getCreateInfo(VkPhysicalDeviceFeatures2 features, std::vector<VkDeviceQueueCreateInfo>& queues, std::vector<const char*>& extensions) const;

public:
  VkDevice handle;
  Queue* graphicsQueue;
  Queue* computeQueue;
  VkPhysicalDeviceFeatures enabledFeatures{};

  Device(PhysicalDevice* physicalDevice, std::vector<const char*> extensions);
  ~Device();

  PhysicalDevice* physicalDevice() const { return _physicalDevicePtr; };
  VkPhysicalDeviceLimits physicalDeviceLimits() const;
  VkPhysicalDeviceProperties physicalDeviceProperties() const;
  VkFormatProperties formatProperties(VkFormat format) const;
  VkImageFormatProperties imageFormatProperties(VkImageCreateInfo* imageFormatInfo) const;

  void waitIdle() const;
};