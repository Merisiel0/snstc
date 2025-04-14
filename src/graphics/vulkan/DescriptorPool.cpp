#include "DescriptorPool.h"

#include "DescriptorSetLayout.h"
#include "Device.h"

VkDescriptorPoolCreateInfo DescriptorPool::getCreateInfo(
  std::vector<VkDescriptorPoolSize>& poolSizes) const {
  VkDescriptorPoolCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;

  for(const VkDescriptorPoolSize& poolSize : poolSizes) {
    info.maxSets += poolSize.descriptorCount;
  }

  info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  info.pPoolSizes = poolSizes.data();

  return info;
}

DescriptorPool::DescriptorPool(std::shared_ptr<Device> device,
  std::vector<VkDescriptorPoolSize> poolSizes) {
  _device = device;

  VkDescriptorPoolCreateInfo createInfo = getCreateInfo(poolSizes);
  VK_CHECK(vkCreateDescriptorPool(_device->handle, &createInfo, nullptr, &handle));
}

DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(_device->handle, handle, nullptr); }

void DescriptorPool::reset() const { vkResetDescriptorPool(_device->handle, handle, 0); }
