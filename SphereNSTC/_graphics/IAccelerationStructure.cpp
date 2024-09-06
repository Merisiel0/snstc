//#include "IAccelerationStructure.h"
//
//#include "Buffer.h"
//
//VkAccelerationStructureCreateInfoKHR& IAccelerationStructure::getCreateInfo(VkDeviceSize size, VkAccelerationStructureTypeKHR type) {
//  VkAccelerationStructureCreateInfoKHR info{};
//  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
//  //info.pNext;
//  //info.createFlags;
//  info.buffer = _memory->handle;
//  //info.offset;
//  info.size = size;
//  info.type = type;
//  //info.deviceAddress;
//
//  return info;
//}
//
//IAccelerationStructure::~IAccelerationStructure() {
//  vkDestroyAccelerationStructureKHR(*_devicePtr, handle, nullptr);
//  delete _memory;
//  delete _scratch;
//}