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
  info.pSetLayouts = new VkDescriptorSetLayout(layout.handle);

  return info;
}

VkWriteDescriptorSet DescriptorSet::getWriteInfo(uint32_t binding, const Image& image,
  const Sampler& sampler) const {
  VkWriteDescriptorSet write {};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  //write.pNext;
  write.dstSet = handle;
  write.dstBinding = binding;
  //write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  write.pImageInfo = new VkDescriptorImageInfo(getImageInfo(image, sampler));
  //write.pBufferInfo;
  //write.pTexelBufferView;

  return write;
}

VkWriteDescriptorSet DescriptorSet::getWriteInfo(uint32_t binding, Buffer* buffer) const {
  VkWriteDescriptorSet write {};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  //write.pNext;
  write.dstSet = handle;
  write.dstBinding = binding;
  //write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  //write.pImageInfo;
  write.pBufferInfo = new VkDescriptorBufferInfo(getBufferInfo(buffer));
  //write.pTexelBufferView;

  return write;
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
  VkWriteDescriptorSet writeInfo = getWriteInfo(binding, image, sampler);
  vkUpdateDescriptorSets(_device->handle, 1, &writeInfo, 0, nullptr);
}

void DescriptorSet::write(uint32_t binding, Buffer* buffer) {
  VkWriteDescriptorSet writeInfo = getWriteInfo(binding, buffer);
  vkUpdateDescriptorSets(_device->handle, 1, &writeInfo, 0, nullptr);
}
