#include "PhysicalDevice.h"

#include "Instance.h"
#include <map>

PhysicalDeviceQueueFamilyIndexes PhysicalDevice::findQueueFamilyIndexes(const VkPhysicalDevice& physicalDevice) {
  uint32_t queueFamilyPropertyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

  PhysicalDeviceQueueFamilyIndexes queueFamilyIndexes{};

  for (uint32_t i = 0; i < queueFamilyPropertyCount; i++) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndexes.graphicsFamilyIndex = i;
      break;
    }
  }

  if (queueFamilyIndexes.graphicsFamilyIndex.has_value()) {
    for (uint32_t i = 0; i < queueFamilyPropertyCount; i++) {
      if (i != queueFamilyIndexes.graphicsFamilyIndex && (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
        queueFamilyIndexes.computeFamilyIndex = i;
        break;
      }
    }
  }

  return queueFamilyIndexes;
}

int PhysicalDevice::rate(VkPhysicalDevice physicalDevice, std::vector<const char*> requiredExtensions) {
  int score{ 0 };

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  uint32_t minorVersion = VK_VERSION_MINOR(properties.apiVersion);
  if (VK_VERSION_MINOR(properties.apiVersion) < 3) return 0;

  switch (properties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    score += 100;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    score += 10;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    score += 1;
    break;
  default:
    return 0;
  }

  //VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
  //rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
  //rayTracingPipelineFeatures.pNext = nullptr;

  //VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
  //accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
  //accelStructFeatures.pNext = &rayTracingPipelineFeatures;

  VkPhysicalDeviceVulkan13Features vk13Features{};
  vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  //vk13Features.pNext = &accelStructFeatures;

  VkPhysicalDeviceVulkan12Features vk12Features{};
  vk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  vk12Features.pNext = &vk13Features;

  VkPhysicalDeviceFeatures2 features{};
  features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
  features.pNext = &vk12Features;

  vkGetPhysicalDeviceFeatures2KHR(physicalDevice, &features);

  if (features.features.logicOp == VK_FALSE
    || vk12Features.bufferDeviceAddress == VK_FALSE
    || vk13Features.dynamicRendering == VK_FALSE
    || vk13Features.synchronization2 == VK_FALSE
    /*|| accelStructFeatures.accelerationStructure == VK_FALSE
    || rayTracingPipelineFeatures.rayTracingPipeline == VK_FALSE*/) return 0;

  PhysicalDeviceQueueFamilyIndexes queueFamilyIndexes = findQueueFamilyIndexes(physicalDevice);
  if (!queueFamilyIndexes.isComplete()) return 0;

  uint32_t deviceExtensionPropertyCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionPropertyCount, nullptr);
  std::vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionPropertyCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionPropertyCount, deviceExtensionProperties.data());

  size_t requiredExtensionsFound{ 0 };
  for (const auto& deviceExtensionProperty : deviceExtensionProperties) {
    for (const auto& requiredExtension : requiredExtensions) {
      if (strcmp(deviceExtensionProperty.extensionName, requiredExtension) == 0) {
        requiredExtensionsFound++;
        score++;
        break;
      }
    }
  }
  if (requiredExtensionsFound < requiredExtensions.size()) return 0;

  return score;
}

PhysicalDevice::PhysicalDevice(Instance* instance, std::vector<const char*> requiredExtensions) {
  size_t requiredExtensionsFound{ 0 };

  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(instance->handle, &physicalDeviceCount, nullptr);
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance->handle, &physicalDeviceCount, physicalDevices.data());

  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto& physicalDevice : physicalDevices) {
    int score = rate(physicalDevice, requiredExtensions);
    candidates.insert(std::make_pair(score, physicalDevice));
  }

  if (candidates.rbegin()->first > 0) {
    handle = candidates.rbegin()->second;
    queueFamilies = findQueueFamilyIndexes(handle);

    //rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    //accelerationStructureProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    //accelerationStructureProperties.pNext = &rayTracingPipelineProperties;

    VkPhysicalDeviceProperties2 physicalDeviceProperties{};
    physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    //physicalDeviceProperties.pNext = &accelerationStructureProperties;
    //physicalDeviceProperties.properties;

    vkGetPhysicalDeviceProperties2(handle, &physicalDeviceProperties);
    properties = physicalDeviceProperties.properties;
  } else {
    throw std::runtime_error("Failed to find a suitable GPU.");
  }
}