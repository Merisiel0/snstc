#include "BLAS.h"

#include "Device.h"
#include "Allocator.h"
#include "ImmediateSubmit.h"
#include "Buffer.h"
#include "Geometry.h"

BLAS::BLAS(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
  _devicePtr = &device->handle;

  // --- GEOMETRY ---
  _geometry = new Geometry(device, allocator, immediateSubmit, vertices, indices);

  // --- BUILD SIZES ---
  uint32_t primitiveCount = static_cast<uint32_t>(vertices.size() / 3);

  _buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  vkGetAccelerationStructureBuildSizesKHR(device->handle, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
    &_geometry->buildInfo, &primitiveCount, &_buildSizesInfo);

  // --- SCRATCH BUFFER ---
  _scratch = new Buffer(device, allocator, _buildSizesInfo.buildScratchSize + 256,
    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  _geometry->buildInfo.scratchData.deviceAddress = _scratch->address + 256 - (_scratch->address % 256);

  // --- MEMORY BUFFER ---
  _memory = new Buffer(device, allocator, _buildSizesInfo.accelerationStructureSize,
    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);

  // --- CREATE AS ---
  VkAccelerationStructureCreateInfoKHR createInfo = getCreateInfo(_buildSizesInfo.accelerationStructureSize,
    VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);
  vkCreateAccelerationStructureKHR(device->handle, &createInfo, nullptr, &handle);
  _geometry->buildInfo.dstAccelerationStructure = handle;

  // --- BUILD RANGE INFO ---
  _buildRangeInfo.primitiveCount = primitiveCount;
  //_buildRangeInfo.primitiveOffset;
  _buildRangeInfo.firstVertex = 0;
  //_buildRangeInfo.transformOffset;

  immediateSubmit->submit([&](VkCommandBuffer cmd) {
    const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &_buildRangeInfo;
    const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
    vkCmdBuildAccelerationStructuresKHR(cmd, 1, &_geometry->buildInfo, cpcvkp);
    });
}

VkAccelerationStructureInstanceKHR BLAS::getInstance(glm::mat4 transform) {
  VkAccelerationStructureInstanceKHR instance{};
  //instance.instanceCustomIndex;
  //instance.mask;
  //instance.instanceShaderBindingTableRecordOffset;
  //instance.flags;
  //instance.accelerationStructureReference;

  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 4; x++) {
      instance.transform.matrix[y][x] = transform[y][x];
    }
  }

  return instance;
}