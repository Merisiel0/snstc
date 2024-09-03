#pragma once

#include "VulkanUtils.h"

class Device;
class Allocator;
class ImmediateSubmit;
class Buffer;

class Geometry {
private:
  std::vector<Buffer*> _buffers{};

public:
  VkAccelerationStructureGeometryKHR handle{};
  VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};

  Geometry(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<Vertex> vertices, std::vector<Index> indices);
  Geometry(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<VkAccelerationStructureInstanceKHR> instances, Buffer*& out);
  ~Geometry();
};