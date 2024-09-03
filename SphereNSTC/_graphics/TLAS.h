#pragma once

#include "VulkanUtils.h"
#include "IAccelerationStructure.h"

class Device;
class Allocator;
class ImmediateSubmit;
class Buffer;

class TLAS : IAccelerationStructure {
private:
  Buffer* _instancesBuffer{ nullptr };

public:
  void build();

  TLAS() {}
  TLAS(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<VkAccelerationStructureInstanceKHR> instances);
  ~TLAS();
};