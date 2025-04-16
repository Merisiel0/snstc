#include "DescriptorSet.h"

#include "Buffer.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "Image.h"
#include "Sampler.h"

VkDescriptorSetAllocateInfo DescriptorSet::getSetAllocateInfo(const DescriptorPool& pool,
  const DescriptorSetLayout& layout) const {
  VkDescriptorSetAllocateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  //info.pNext;
  info.descriptorPool = pool.handle;
  info.descriptorSetCount = 1;
  info.pSetLayouts = &layout.handle;

  return info;
}

WriteDescriptorSetData DescriptorSet::getWriteInfo(uint32_t binding, const Image& image,
  const Sampler& sampler) const {
  WriteDescriptorSetData data;

  data.imageInfo = getImageInfo(image, sampler);

  data.info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  data.info.pNext = nullptr;
  data.info.dstSet = handle;
  data.info.dstBinding = binding;
  data.info.dstArrayElement = 0;
  data.info.descriptorCount = 1;
  data.info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  data.info.pImageInfo = &data.imageInfo;
  data.info.pBufferInfo = nullptr;
  data.info.pTexelBufferView = nullptr;

  return data;
}

WriteDescriptorSetData DescriptorSet::getWriteInfo(uint32_t binding, Buffer* buffer) const {
  WriteDescriptorSetData data;

  data.bufferInfo = getBufferInfo(buffer);

  data.info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  data.info.pNext = nullptr;
  data.info.dstSet = handle;
  data.info.dstBinding = binding;
  data.info.dstArrayElement = 0;
  data.info.descriptorCount = 1;
  data.info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  data.info.pImageInfo = nullptr;
  data.info.pBufferInfo = &data.bufferInfo;
  data.info.pTexelBufferView = nullptr;

  return data;
}

VkDescriptorImageInfo DescriptorSet::getImageInfo(const Image& image,
  const Sampler& sampler) const {
  VkDescriptorImageInfo info {};
  info.sampler = sampler.handle;
  info.imageView = image.view;
  info.imageLayout = image.layout();

  return info;
}

VkDescriptorBufferInfo DescriptorSet::getBufferInfo(Buffer* buffer) const {
  VkDescriptorBufferInfo info {};
  info.buffer = buffer->handle;
  info.offset = 0;
  info.range = VK_WHOLE_SIZE;

  return info;
}

DescriptorSet::DescriptorSet(std::shared_ptr<Device> device, const DescriptorPool& pool,
  const DescriptorSetLayout& layout) {
  _device = device;

  VkDescriptorSetAllocateInfo allocateInfo = getSetAllocateInfo(pool, layout);
  VkDescriptorSet descriptorSet {};
  VK_CHECK(vkAllocateDescriptorSets(_device->handle, &allocateInfo, &handle));
}

void DescriptorSet::write(uint32_t binding, const Image& image, const Sampler& sampler) {
  WriteDescriptorSetData writeData = getWriteInfo(binding, image, sampler);
  vkUpdateDescriptorSets(_device->handle, 1, &writeData.info, 0, nullptr);
}

void DescriptorSet::write(uint32_t binding, Buffer* buffer) {
  WriteDescriptorSetData writeData = getWriteInfo(binding, buffer);
  vkUpdateDescriptorSets(_device->handle, 1, &writeData.info, 0, nullptr);
}
