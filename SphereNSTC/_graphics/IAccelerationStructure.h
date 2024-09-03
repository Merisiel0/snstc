#pragma once

#include "VulkanUtils.h"

class Geometry;
class Buffer;

class IAccelerationStructure {
protected:
  VkDevice* _devicePtr{ nullptr };
  Geometry* _geometry{ nullptr };
  Buffer* _memory{ nullptr };
  Buffer* _scratch{ nullptr };

  VkAccelerationStructureBuildSizesInfoKHR _buildSizesInfo{};
  VkAccelerationStructureBuildRangeInfoKHR _buildRangeInfo{};

  VkAccelerationStructureCreateInfoKHR& getCreateInfo(VkDeviceSize size, VkAccelerationStructureTypeKHR type);

  /*VkAccelerationStructureBuildGeometryInfoKHR& getBuildGeometryInfo(VkAccelerationStructureTypeKHR type, bool complete = true,
    VkBuildAccelerationStructureModeKHR mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR);

  VkAccelerationStructureBuildSizesInfoKHR& getBuildSizesInfo(Device* device);

  VkAccelerationStructureBuildRangeInfoKHR& getBuildRangeInfo(uint32_t primitiveCount = 1);*/

  //void uploadToDevice(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, VkAccelerationStructureTypeKHR type);

public:
  VkAccelerationStructureKHR handle{ VK_NULL_HANDLE };

  IAccelerationStructure() {}
  ~IAccelerationStructure();
};