#pragma once

#include "VulkanUtils.h"

class Device;
class DescriptorPool;
class DescriptorSetLayout;
class Image;
class Sampler;
class Buffer;

class DescriptorSet {
  private:
  std::shared_ptr<Device> _device;

  VkDescriptorSetAllocateInfo getSetAllocateInfo(const DescriptorPool& pool, const DescriptorSetLayout& layout) const;
  VkWriteDescriptorSet getWriteInfo(uint32_t binding, const Image& image, const Sampler& sampler) const;
  VkWriteDescriptorSet getWriteInfo(uint32_t binding, Buffer* buffer) const;
  VkDescriptorImageInfo getImageInfo(const Image& image, const Sampler& sampler) const;
  VkDescriptorBufferInfo getBufferInfo(Buffer* buffer) const;

  public:
  VkDescriptorSet handle;

  DescriptorSet(std::shared_ptr<Device> device, const DescriptorPool& pool, const DescriptorSetLayout& layout);

  void write(uint32_t binding, const Image& image, const Sampler& sampler);
  void write(uint32_t binding, Buffer* buffer);
};