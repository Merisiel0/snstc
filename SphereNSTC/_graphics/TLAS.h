//#pragma once
//
//#include "VulkanUtils.h"
//#include "IAccelerationStructure.h"
//
//class Device;
//class Allocator;
//class ImmediateSubmit;
//class Buffer;
//
//class TLAS : IAccelerationStructure {
//private:
//  Buffer* _instancesBuffer{ nullptr };
//
//public:
//  void build();
//
//  TLAS() {}
//  TLAS(ImmediateSubmit* immediateSubmit, std::vector<VkAccelerationStructureInstanceKHR> instances);
//  ~TLAS();
//};