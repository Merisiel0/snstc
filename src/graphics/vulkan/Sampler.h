#pragma once

#include "VulkanUtils.h"

class Device;

class Sampler {
private:
  Device* _devicePtr;

  VkSamplerCreateInfo getCreateInfo() const;

public:
  VkSampler handle;

  Sampler(Device* device);
  ~Sampler();
};