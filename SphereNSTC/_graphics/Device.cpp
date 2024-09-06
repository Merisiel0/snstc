#include "Device.h"

#include "PhysicalDevice.h"
#include "Queue.h"

VkDeviceCreateInfo& Device::getCreateInfo(VkPhysicalDeviceFeatures2 features, std::vector<VkDeviceQueueCreateInfo>& queues, std::vector<const char*>& extensions) {
  VkDeviceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pNext = &features;
  info.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
  info.pQueueCreateInfos = queues.data();
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();
  //info.pEnabledFeatures

  return info;
}

Device::Device(PhysicalDevice* physicalDevice, std::vector<const char*> extensions) {
  std::vector<uint32_t> queueFamilyIndexes = physicalDevice->queueFamilies.getIndexes();

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;

  for (size_t i = 0; i < queueFamilyIndexes.size(); i++) {
    VkDeviceQueueCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    //createInfo.pNext;
    //createInfo.flags;
    createInfo.queueFamilyIndex = queueFamilyIndexes[i];
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(createInfo);
  }

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
  rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
  //rayTracingPipelineFeatures.pNext = nullptr;
  rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;

  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
  accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
  accelStructFeatures.pNext = &rayTracingPipelineFeatures;
  accelStructFeatures.accelerationStructure = VK_TRUE;

  VkPhysicalDeviceVulkan13Features vk13Features{};
  vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  vk13Features.pNext = &accelStructFeatures;
  vk13Features.synchronization2 = VK_TRUE;
  vk13Features.dynamicRendering = VK_TRUE;

  VkPhysicalDeviceVulkan12Features vk12Features{};
  vk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  vk12Features.pNext = &vk13Features;
  vk12Features.bufferDeviceAddress = VK_TRUE;

  VkPhysicalDeviceFeatures2 features{};
  features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
  features.pNext = &vk12Features;
  features.features.logicOp = VK_TRUE;

  VkDeviceCreateInfo createInfo = getCreateInfo(features, queueCreateInfos, extensions);

  vkCreateDevice(physicalDevice->handle, &createInfo, nullptr, &handle);

  graphicsQueue = new Queue(this, physicalDevice->queueFamilies.graphicsFamilyIndex.value());
  computeQueue = new Queue(this, physicalDevice->queueFamilies.computeFamilyIndex.value());
}

Device::~Device() {
  delete graphicsQueue;
  delete computeQueue;

  vkDestroyDevice(handle, nullptr);
}

void Device::waitIdle() const {
  VK_CHECK(vkDeviceWaitIdle(handle));
}