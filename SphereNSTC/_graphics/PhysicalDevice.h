#pragma once

#include "VulkanUtils.h"
#include "optional"

class Instance;

struct PhysicalDeviceQueueFamilyIndexes {
  std::optional<uint32_t> graphicsFamilyIndex;
  std::optional<uint32_t> computeFamilyIndex;

  bool isComplete() const {
    return graphicsFamilyIndex.has_value() && computeFamilyIndex.has_value();
  }

  std::vector<uint32_t> getIndexes() {
    std::vector<uint32_t> indexes;

    if (graphicsFamilyIndex.has_value()) indexes.push_back(graphicsFamilyIndex.value());
    if (computeFamilyIndex.has_value()) indexes.push_back(computeFamilyIndex.value());

    return indexes;
  }
};

class PhysicalDevice {
private:
  int rate(VkPhysicalDevice physicalDevice, std::vector<const char*> requiredExtensions);
  PhysicalDeviceQueueFamilyIndexes findQueueFamilyIndexes(const VkPhysicalDevice& physicalDevice);

public:
  VkPhysicalDevice handle{ VK_NULL_HANDLE };
  PhysicalDeviceQueueFamilyIndexes queueFamilies;

  VkPhysicalDeviceProperties properties{};
  VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructureProperties{};
  VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};

  PhysicalDevice() {}
  PhysicalDevice(Instance* instance, std::vector<const char*> requiredExtensions);
  ~PhysicalDevice() {};
};