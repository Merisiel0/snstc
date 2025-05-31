#pragma once

#include "../../utils/Utils.h"
#include "DataStructures.h"
#include "SDL3/SDL_vulkan.h"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.h"

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

enum WindowScreenMode { WINDOWED = 0, FULLSCREEN = SDL_WINDOW_FULLSCREEN };

enum MeshLayout : uint16_t {
  MESH_LAYOUT_STATIC = 0,
  MESH_LAYOUT_STATIC_INSTANCE = 1,
  MESH_LAYOUT_PARTICLE = 2,
  MESH_LAYOUT_COUNT = 3,
  MESH_LAYOUT_UNDEFINED = 0xFFFF
};

enum LightingType : uint16_t {
  LIGHTING_TYPE_UNLIT = 0,
  LIGHTING_TYPE_PHONG = 1,
  LIGHTING_TYPE_BLINN_PHONG = 2,
  LIGHTING_TYPE_COOK_TORRANCE = 3,
  LIGHTING_TYPE_COUNT = 4,
  LIGHTING_TYPE_UNDEFINED = 0xFFFF
};

enum UniqueGraphicsPipeline : uint16_t {
  UNIQUE_GRAPHICS_PIPELINE_SKYBOX = 1,
  UNIQUE_GRAPHICS_PIPELINE_UNDEFINED = 0xFFFF
};

// /// @brief List containing all implemented graphics pipelines.
// enum GraphicsPipelineId {
//   /// @brief Reserved for skyboxes.
//   GRAPHICS_PIPELINE_SKYBOX,

//   // UNLIT_POINT_LIST,
//   // UNLIT_LINE_LIST,
//   // UNLIT_LINE_STRIP,

//   // UNLIT_POINT_TRIANGLE_LIST,
//   UNLIT_LINE_TRIANGLE_LIST,
//   UNLIT_FILL_TRIANGLE_LIST,

//   // UNLIT_POINT_TRIANGLE_STRIP,
//   // UNLIT_LINE_TRIANGLE_STRIP,
//   // UNLIT_FILL_TRIANGLE_STRIP,

//   // UNLIT_POINT_TRIANGLE_FAN,
//   // UNLIT_LINE_TRIANGLE_FAN,
//   // UNLIT_FILL_TRIANGLE_FAN,

//   // UNLIT_INSTANCE_POINT_LIST,
//   // UNLIT_INSTANCE_LINE_LIST,
//   // UNLIT_INSTANCE_LINE_STRIP,

//   // UNLIT_INSTANCE_POINT_TRIANGLE_LIST,
//   // UNLIT_INSTANCE_LINE_TRIANGLE_LIST,
//   // UNLIT_INSTANCE_FILL_TRIANGLE_LIST,

//   // UNLIT_INSTANCE_POINT_TRIANGLE_STRIP,
//   // UNLIT_INSTANCE_LINE_TRIANGLE_STRIP,
//   // UNLIT_INSTANCE_FILL_TRIANGLE_STRIP,

//   // UNLIT_INSTANCE_POINT_TRIANGLE_FAN,
//   // UNLIT_INSTANCE_LINE_TRIANGLE_FAN,
//   // UNLIT_INSTANCE_FILL_TRIANGLE_FAN,

//   // UNLIT_PARTICLE_POINT_LIST,
//   // UNLIT_PARTICLE_LINE_LIST,
//   // UNLIT_PARTICLE_LINE_STRIP,

//   // UNLIT_PARTICLE_POINT_TRIANGLE_LIST,
//   // UNLIT_PARTICLE_LINE_TRIANGLE_LIST,
//   // UNLIT_PARTICLE_FILL_TRIANGLE_LIST,

//   // UNLIT_PARTICLE_POINT_TRIANGLE_STRIP,
//   // UNLIT_PARTICLE_LINE_TRIANGLE_STRIP,
//   // UNLIT_PARTICLE_FILL_TRIANGLE_STRIP,

//   // UNLIT_PARTICLE_POINT_TRIANGLE_FAN,
//   // UNLIT_PARTICLE_LINE_TRIANGLE_FAN,
//   // UNLIT_PARTICLE_FILL_TRIANGLE_FAN,

//   GRAPHICS_PIPELINE_ID_COUNT
// };