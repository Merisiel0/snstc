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
  Device* _devicePtr;
  DescriptorPool* _poolPtr;

  VkDescriptorSetAllocateInfo getSetAllocateInfo(DescriptorPool* pool, DescriptorSetLayout* layout) const;
  VkWriteDescriptorSet getWriteInfo(uint32_t binding, Image* image, Sampler* sampler) const;
  VkWriteDescriptorSet getWriteInfo(uint32_t binding, Buffer* buffer) const;
  VkDescriptorImageInfo getImageInfo(Image* image, Sampler* sampler) const;
  VkDescriptorBufferInfo getBufferInfo(Buffer* buffer) const;

public:
  VkDescriptorSet handle;

  DescriptorSet(Device* device, DescriptorPool* pool, DescriptorSetLayout* layout);

  void write(uint32_t binding, Image* image, Sampler* sampler);
  void write(uint32_t binding, Buffer* buffer);
};