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
  std::shared_ptr<DescriptorPool> _pool;
  const DescriptorSetLayout* _layout;

  VkDescriptorSet _handle;

  DescriptorSetAllocateInfoData getSetAllocateInfo(const DescriptorPool& pool,
    const DescriptorSetLayout& layout) const;
  WriteDescriptorSetData getWriteInfo(uint32_t binding, const Image& image,
    const Sampler& sampler) const;
  WriteDescriptorSetData getWriteInfo(uint32_t binding, const Buffer& buffer) const;
  VkDescriptorImageInfo getImageInfo(const Image& image, const Sampler& sampler) const;
  VkDescriptorBufferInfo getBufferInfo(const Buffer& buffer) const;

public:
  VkDescriptorSet getHandle() const;

  DescriptorSet(std::shared_ptr<Device> device, std::shared_ptr<DescriptorPool> pool,
    const DescriptorSetLayout& layout);
  ~DescriptorSet();

  void write(uint32_t binding, const Image& image, const Sampler& sampler);
  void write(uint32_t binding, const Buffer& buffer);
};