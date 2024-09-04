#pragma once

#include "VulkanUtils.h"

class Device;

class IPipeline {
protected:
  VkDevice* _devicePtr{ nullptr };
  VkPipelineLayout _layout{ VK_NULL_HANDLE };

public:
  VkPipeline handle{ VK_NULL_HANDLE };

  IPipeline(Device* device);
  ~IPipeline();

  VkPipelineLayout layout() const { return _layout; }
};