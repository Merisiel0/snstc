#include "DescriptorSet.h"

#include "Device.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Image.h"
#include "Sampler.h"
#include "Buffer.h"

VkDescriptorSetAllocateInfo DescriptorSet::getSetAllocateInfo(DescriptorPool* pool, DescriptorSetLayout* layout) const {
  VkDescriptorSetAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  //info.pNext;
  info.descriptorPool = pool->handle;
  info.descriptorSetCount = 1;
  info.pSetLayouts = new VkDescriptorSetLayout(layout->handle);

  return info;
}

VkWriteDescriptorSet DescriptorSet::getWriteInfo(uint32_t binding, Image* image, Sampler* sampler) const {
  VkWriteDescriptorSet write{};
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
  VkWriteDescriptorSet write{};
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

VkDescriptorImageInfo DescriptorSet::getImageInfo(Image* image, Sampler* sampler) const {
  VkDescriptorImageInfo info{};
  info.sampler = sampler->handle;
  info.imageView = image->view;
  info.imageLayout = image->layout();

  return info;
}

VkDescriptorBufferInfo DescriptorSet::getBufferInfo(Buffer* buffer) const {
  VkDescriptorBufferInfo info{};
  info.buffer = buffer->handle;
  info.offset = 0;
  info.range = VK_WHOLE_SIZE;

  return info;
}

DescriptorSet::DescriptorSet(Device* device, DescriptorPool* pool, DescriptorSetLayout* layout) {
  _devicePtr = device;
  _poolPtr = pool;

  VkDescriptorSetAllocateInfo allocateInfo = getSetAllocateInfo(pool, layout);
  VkDescriptorSet descriptorSet{};
  VK_CHECK(vkAllocateDescriptorSets(_devicePtr->handle, &allocateInfo, &handle));
}

void DescriptorSet::write(uint32_t binding, Image* image, Sampler* sampler) {
  VkWriteDescriptorSet writeInfo = getWriteInfo(binding, image, sampler);
  vkUpdateDescriptorSets(_devicePtr->handle, 1, &writeInfo, 0, nullptr);
}

void DescriptorSet::write(uint32_t binding, Buffer* buffer) {
  VkWriteDescriptorSet writeInfo = getWriteInfo(binding, buffer);
  vkUpdateDescriptorSets(_devicePtr->handle, 1, &writeInfo, 0, nullptr);
}
