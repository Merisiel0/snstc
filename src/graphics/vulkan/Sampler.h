#pragma once

#include "VulkanUtils.h"

class Device;

class Sampler {
private:
  std::shared_ptr<Device> _device;
  VkSampler _handle;

  VkSamplerCreateInfo getCreateInfo() const;

public:
  const VkSampler& getHandle() const;

  Sampler(std::shared_ptr<Device> device);
  ~Sampler();
};