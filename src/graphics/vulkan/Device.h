#pragma once

#include "VulkanUtils.h"

class PhysicalDevice;
class Queue;

class Device {
private:
  std::shared_ptr<PhysicalDevice> _physicalDevice;

  VkDeviceCreateInfo getCreateInfo(VkPhysicalDeviceFeatures2& features,
    std::vector<VkDeviceQueueCreateInfo>& queues, std::vector<const char*>& extensions) const;

public:
  VkDevice handle;
  std::shared_ptr<Queue> graphicsQueue;
  std::shared_ptr<Queue> computeQueue;
  VkPhysicalDeviceFeatures enabledFeatures {};

  Device(std::shared_ptr<PhysicalDevice> physicalDevice, std::vector<const char*> extensions);
  ~Device();

  std::shared_ptr<PhysicalDevice> physicalDevice() const { return _physicalDevice; };
  VkPhysicalDeviceLimits physicalDeviceLimits() const;
  VkPhysicalDeviceProperties physicalDeviceProperties() const;
  VkFormatProperties formatProperties(VkFormat format) const;
  VkImageFormatProperties imageFormatProperties(VkImageCreateInfo* imageFormatInfo) const;

  void waitIdle() const;
};