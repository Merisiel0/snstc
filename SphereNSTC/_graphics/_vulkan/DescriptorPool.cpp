#include "DescriptorPool.h"

#include "Device.h"
#include "DescriptorSetLayout.h"

VkDescriptorPoolCreateInfo DescriptorPool::getCreateInfo(std::vector<VkDescriptorPoolSize> poolSizes) const {
  VkDescriptorPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;

  for (const VkDescriptorPoolSize& poolSize : poolSizes) {
    info.maxSets += poolSize.descriptorCount;
  }

  info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  VkDescriptorPoolSize* poolSizesPtr = new VkDescriptorPoolSize[info.poolSizeCount];
  std::copy(poolSizes.begin(), poolSizes.end(), poolSizesPtr);
  info.pPoolSizes = poolSizesPtr;

  return info;
}

DescriptorPool::DescriptorPool(Device* device, std::vector<VkDescriptorPoolSize> poolSizes) {
  _devicePtr = &device->handle;

  VkDescriptorPoolCreateInfo createInfo = getCreateInfo(poolSizes);
  VK_CHECK(vkCreateDescriptorPool(*_devicePtr, &createInfo, nullptr, &handle));
}

DescriptorPool::~DescriptorPool() {
  vkDestroyDescriptorPool(*_devicePtr, handle, nullptr);
}

void DescriptorPool::reset() const {
  vkResetDescriptorPool(*_devicePtr, handle, 0);
}
