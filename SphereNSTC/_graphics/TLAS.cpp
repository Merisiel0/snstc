//#include "TLAS.h"
//
//#include "ImmediateSubmit.h"
//#include "Buffer.h"
//#include "Geometry.h"
//
//void TLAS::build() {
//}
//
//TLAS::TLAS(ImmediateSubmit* immediateSubmit, std::vector<VkAccelerationStructureInstanceKHR> instances) {
//  // --- GEOMETRY ---
//  _geometry = new Geometry(instances, _instancesBuffer);
//
//  // --- BUILD SIZES ---
//  uint32_t primitiveCount = static_cast<uint32_t>(instances.size());
//
//  _buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
//  vkGetAccelerationStructureBuildSizesKHR(device->handle, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
//    &_geometry->buildInfo, &primitiveCount, &_buildSizesInfo);
//
//  // --- SCRATCH BUFFER ---
//  _scratch = new Buffer(_buildSizesInfo.buildScratchSize + 256,
//    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
//    VMA_MEMORY_USAGE_GPU_ONLY);
//  _geometry->buildInfo.scratchData.deviceAddress = _scratch->address + 256 - (_scratch->address % 256);
//
//  // --- MEMORY BUFFER ---
//  _memory = new Buffer(_buildSizesInfo.accelerationStructureSize,
//    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);
//
//  // --- CREATE AS ---
//  VkAccelerationStructureCreateInfoKHR createInfo = getCreateInfo(_buildSizesInfo.accelerationStructureSize,
//    VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);
//  vkCreateAccelerationStructureKHR(device->handle, &createInfo, nullptr, &handle);
//  _geometry->buildInfo.dstAccelerationStructure = handle;
//
//  // --- BUILD RANGE INFO ---
//  _buildRangeInfo.primitiveCount = primitiveCount;
//  //_buildRangeInfo.primitiveOffset;
//  _buildRangeInfo.firstVertex = 0;
//  //_buildRangeInfo.transformOffset;
//
//  immediateSubmit->submit([&](CommandBuffer* cmd) {
//    const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &_buildRangeInfo;
//    const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
//    vkCmdBuildAccelerationStructuresKHR(cmd->handle, 1, &_geometry->buildInfo, cpcvkp);
//    });
//}
//
//TLAS::~TLAS() {
//  delete _instancesBuffer;
//}