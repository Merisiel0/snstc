#include "DescriptorSet.h"

#include "Buffer.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "Image.h"
#include "Sampler.h"
#include "VulkanHandler.h"
#include "DescriptorManager.h"

DescriptorSetAllocateInfoData DescriptorSet::getSetAllocateInfo(const DescriptorPool& pool,
  const DescriptorSetLayout& layout) const {
  DescriptorSetAllocateInfoData data;
  data.setLayouts.push_back(layout.getHandle());

  data.info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  data.info.pNext = nullptr;
  data.info.descriptorPool = pool.getHandle();
  data.info.descriptorSetCount = 1;
  data.info.pSetLayouts = data.setLayouts.data();

  return data;
}

WriteDescriptorSetData DescriptorSet::getWriteInfo(uint32_t binding, const Image& image,
  const Sampler& sampler) const {
  WriteDescriptorSetData data;

  data.imageInfo = getImageInfo(image, sampler);

  data.info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  data.info.pNext = nullptr;
  data.info.dstSet = _handle;
  data.info.dstBinding = binding;
  data.info.dstArrayElement = 0;
  data.info.descriptorCount = 1;
  data.info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  data.info.pImageInfo = &data.imageInfo;
  data.info.pBufferInfo = nullptr;
  data.info.pTexelBufferView = nullptr;

  return data;
}

WriteDescriptorSetData DescriptorSet::getWriteInfo(uint32_t binding, const Buffer& buffer) const {
  WriteDescriptorSetData data;

  data.bufferInfo = getBufferInfo(buffer);

  data.info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  data.info.pNext = nullptr;
  data.info.dstSet = _handle;
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
  VkDescriptorImageInfo info;
  info.sampler = sampler.getHandle();
  info.imageView = image.view;
  info.imageLayout = image.layout();

  return info;
}

VkDescriptorBufferInfo DescriptorSet::getBufferInfo(const Buffer& buffer) const {
  VkDescriptorBufferInfo info;
  info.buffer = buffer.handle;
  info.offset = 0;
  info.range = VK_WHOLE_SIZE;

  return info;
}

VkDescriptorSet DescriptorSet::getHandle() const { return _handle; }

DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorPool> pool,
  const DescriptorSetLayout& layout) {
  _pool = pool;
  _layout = &layout;

  pool->addSizes(layout.getPoolSizes());

  DescriptorSetAllocateInfoData allocateInfo = getSetAllocateInfo(*pool, layout);
  VkDescriptorSet descriptorSet {};
  VK_CHECK(vkAllocateDescriptorSets(VulkanHandler::getDevice()->handle, &allocateInfo.info, &_handle));
}

DescriptorSet::~DescriptorSet(){
  _pool->removeSizes(_layout->getPoolSizes());
  DescriptorManager::cleanupExpired();
}

void DescriptorSet::write(uint32_t binding, const Image& image, const Sampler& sampler) {
  WriteDescriptorSetData writeData = getWriteInfo(binding, image, sampler);
  vkUpdateDescriptorSets(VulkanHandler::getDevice()->handle, 1, &writeData.info, 0, nullptr);
}

void DescriptorSet::write(uint32_t binding, const Buffer& buffer) {
  WriteDescriptorSetData writeData = getWriteInfo(binding, buffer);
  vkUpdateDescriptorSets(VulkanHandler::getDevice()->handle, 1, &writeData.info, 0, nullptr);
}
