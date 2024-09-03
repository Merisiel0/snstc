#include "Pipeline.h"

#include "Device.h"

IPipeline::IPipeline(Device* device) {
  _devicePtr = &device->handle;
}

IPipeline::~IPipeline() {
  vkDestroyPipeline(*_devicePtr, handle, nullptr);
  vkDestroyPipelineLayout(*_devicePtr, _layout, nullptr);
}