#pragma once

#include "VulkanUtils.h"

class Device;

class Sampler {
  private:
  std::shared_ptr<Device> _device;

  VkSamplerCreateInfo getCreateInfo() const;

  public:
  VkSampler handle;

  Sampler(std::shared_ptr<Device> device);
  ~Sampler();
};