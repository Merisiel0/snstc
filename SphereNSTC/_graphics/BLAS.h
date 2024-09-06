//#pragma once
//
//#include "IAccelerationStructure.h"
//
//class Device;
//class Allocator;
//class ImmediateSubmit;
//class Buffer;
//
//class BLAS : public IAccelerationStructure {
//public:
//  BLAS() {}
//  BLAS(ImmediateSubmit* immediateSubmit, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
//
//  VkAccelerationStructureInstanceKHR getInstance(glm::mat4 transform);
//};