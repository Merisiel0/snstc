#pragma once

#include "../../utils/Utils.h"
#include "SDL3/SDL_vulkan.h"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.h"
#include "DataStructures.h"

#include <iostream>
#include <memory>

const char* vkResultToString(VkResult result);

#define VK_CHECK(x)                                                                 \
  do {                                                                              \
    VkResult err = x;                                                               \
    if(err != VK_SUCCESS) {                                                         \
      std::cerr << "Detected Vulkan error: " << vkResultToString(err) << std::endl; \
    }                                                                               \
  } while(0)

#ifdef _DEBUG

#  define VKDEBUG

#endif // _DEBUG

#ifdef VKDEBUG
// VK_EXT_debug_utils
#  define vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_
inline PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_ = nullptr;
#  define vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_
inline PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_ = nullptr;
#endif

// VK_KHR_get_physical_device_properties2
#define vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR_
inline PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR_ = nullptr;

// VK_KHR_acceleration_structure
#define vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR_
inline PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR_ =
  nullptr;
#define vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR_
inline PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR_ = nullptr;
#define vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR_
inline PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR_ = nullptr;
#define vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR_
inline PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR_ = nullptr;

// VK_KHR_ray_tracing_pipeline
#define vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR_
inline PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR_ = nullptr;

static void fetchExtensionFunctionPointers(VkInstance instance) {
#ifdef VKDEBUG
  vkCreateDebugUtilsMessengerEXT_ = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
    instance, "vkCreateDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT_ = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
    instance, "vkDestroyDebugUtilsMessengerEXT");
#endif

  vkGetPhysicalDeviceFeatures2KHR_ = (PFN_vkGetPhysicalDeviceFeatures2KHR) vkGetInstanceProcAddr(
    instance, "vkGetPhysicalDeviceFeatures2KHR");

  vkGetAccelerationStructureBuildSizesKHR_ =
    (PFN_vkGetAccelerationStructureBuildSizesKHR) vkGetInstanceProcAddr(instance,
      "vkGetAccelerationStructureBuildSizesKHR");
  vkCreateAccelerationStructureKHR_ = (PFN_vkCreateAccelerationStructureKHR) vkGetInstanceProcAddr(
    instance, "vkCreateAccelerationStructureKHR");
  vkDestroyAccelerationStructureKHR_ =
    (PFN_vkDestroyAccelerationStructureKHR) vkGetInstanceProcAddr(instance,
      "vkDestroyAccelerationStructureKHR");
  vkCmdBuildAccelerationStructuresKHR_ =
    (PFN_vkCmdBuildAccelerationStructuresKHR) vkGetInstanceProcAddr(instance,
      "vkCmdBuildAccelerationStructuresKHR");

  vkCreateRayTracingPipelinesKHR_ = (PFN_vkCreateRayTracingPipelinesKHR) vkGetInstanceProcAddr(
    instance, "vkCreateRayTracingPipelinesKHR");

  if(
#ifdef VKDEBUG
    vkCreateDebugUtilsMessengerEXT_ == nullptr || vkDestroyDebugUtilsMessengerEXT_ == nullptr ||
#endif

    vkGetPhysicalDeviceFeatures2KHR_ == nullptr ||

    vkGetAccelerationStructureBuildSizesKHR_ == nullptr ||
    vkCreateAccelerationStructureKHR_ == nullptr || vkDestroyAccelerationStructureKHR_ == nullptr ||
    vkCmdBuildAccelerationStructuresKHR_ == nullptr ||

    vkCreateRayTracingPipelinesKHR_ == nullptr) {
    VK_CHECK(VK_ERROR_EXTENSION_NOT_PRESENT);
  }
}