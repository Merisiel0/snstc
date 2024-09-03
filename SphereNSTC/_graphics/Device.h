#pragma once

#include "VulkanUtils.h"

class PhysicalDevice;
class Queue;

class Device {
private:
  VkDeviceCreateInfo& getCreateInfo(VkPhysicalDeviceFeatures2 features, std::vector<VkDeviceQueueCreateInfo>& queues, std::vector<const char*>& extensions);

public:
  VkDevice handle{ VK_NULL_HANDLE };
  Queue* graphicsQueue{ nullptr };
  Queue* computeQueue{ nullptr };

  Device() {}
  Device(PhysicalDevice* physicalDevice, std::vector<const char*> extensions);
  ~Device();

  void waitIdle() const;
};