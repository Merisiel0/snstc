#include "Geometry.h"

#include "Buffer.h"

Geometry::Geometry(std::vector<Vertex> vertices, std::vector<Index> indices) {
  handle.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  //_geometry.pNext;
  handle.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  handle.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

  _buffers.push_back(new Buffer(vertices,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
    VMA_MEMORY_USAGE_GPU_ONLY));
  _buffers.push_back(new Buffer(indices,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
    VMA_MEMORY_USAGE_GPU_ONLY));

  handle.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  //handle.geometry.triangles.pNext;
  handle.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  handle.geometry.triangles.vertexData.deviceAddress = _buffers[0]->address;
  handle.geometry.triangles.vertexStride = sizeof(Vertex);
  handle.geometry.triangles.maxVertex = static_cast<uint32_t>(vertices.size() - 1);
  handle.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
  handle.geometry.triangles.indexData.deviceAddress = _buffers[1]->address;
  //handle.geometry.triangles.transformData; //TODO: implement transforms

  buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  //buildInfo.pNext;
  buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  //buildInfo.srcAccelerationStructure;
  buildInfo.geometryCount = 1;
  buildInfo.pGeometries = &handle;
  //buildInfo.ppGeometries;
  //buildInfo.scratchData;
}

Geometry::Geometry(std::vector<VkAccelerationStructureInstanceKHR> instances, Buffer*& out) {
  handle.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  //handle.pNext;
  handle.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  handle.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

  _buffers.resize(instances.size());
  std::vector<VkDeviceAddress> addresses(instances.size());
  for (size_t i = 0; i < instances.size(); i++) {
    std::vector<VkAccelerationStructureInstanceKHR> instance = { instances[i] };
    _buffers[i] = new Buffer(instance,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
      VMA_MEMORY_USAGE_GPU_ONLY);
    addresses[i] = _buffers[i]->address;
  }
  out = new Buffer(_buffers,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
    VMA_MEMORY_USAGE_GPU_ONLY);

  handle.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  //handle.geometry.instances.pNext;
  handle.geometry.instances.arrayOfPointers = VK_FALSE;
  handle.geometry.instances.data.deviceAddress = out->address;

  buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  //buildInfo.pNext;
  buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  //buildInfo.srcAccelerationStructure;
  buildInfo.geometryCount = static_cast<uint32_t>(instances.size());
  buildInfo.pGeometries = &handle;
  //buildInfo.ppGeometries;
}

Geometry::~Geometry() {
  for (auto& buffer : _buffers) {
    delete buffer;
  }
}
