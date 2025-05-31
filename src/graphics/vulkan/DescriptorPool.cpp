#include "DescriptorPool.h"

#include "DescriptorSetLayout.h"
#include "Device.h"
#include "VulkanHandler.h"

VkDescriptorPoolCreateInfo DescriptorPool::getCreateInfo() const {
  VkDescriptorPoolCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;

  info.maxSets = 0;
  for(const VkDescriptorPoolSize& poolSize : _totalSizes) {
    info.maxSets += poolSize.descriptorCount;
  }

  info.poolSizeCount = static_cast<uint32_t>(_totalSizes.size());
  info.pPoolSizes = _totalSizes.data();

  return info;
}

VkDescriptorPool DescriptorPool::getHandle() const { return _handle; }

std::vector<VkDescriptorPoolSize> DescriptorPool::getTotalSizes() const { return _totalSizes; }

std::vector<VkDescriptorPoolSize> DescriptorPool::getUsedSizes() const { return _usedSizes; }

std::vector<VkDescriptorPoolSize> DescriptorPool::getFreeSizes() const {
  std::vector<VkDescriptorPoolSize> freeSizes = _totalSizes;

  for(auto& freeSize : freeSizes) {
    for(const auto& usedSize : _usedSizes) {
      if(usedSize.type == freeSize.type) {
        freeSize.descriptorCount -= usedSize.descriptorCount;
        break;
      }
    }
  }

  return freeSizes;
}

void DescriptorPool::addSizes(std::vector<VkDescriptorPoolSize> sizes) {
  for(const auto& size : sizes) {
    for(auto& usedSize : _usedSizes) {
      if(usedSize.type == size.type) {
        usedSize.descriptorCount += size.descriptorCount;
        break;
      }
    }
  }
}

void DescriptorPool::removeSizes(std::vector<VkDescriptorPoolSize> sizes) {
  for(const auto& size : sizes) {
    for(auto& usedSize : _usedSizes) {
      if(usedSize.type == size.type) { usedSize.descriptorCount -= size.descriptorCount; }
    }
  }
}

DescriptorPool::DescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes) {
  _totalSizes = std::move(poolSizes);

  _usedSizes = _totalSizes;
  for(auto& size : _usedSizes) {
    size.descriptorCount = 0;
  }

  VkDescriptorPoolCreateInfo createInfo = getCreateInfo();
  VK_CHECK(
    vkCreateDescriptorPool(VulkanHandler::getDevice()->handle, &createInfo, nullptr, &_handle));
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) :
    _handle {other._handle}, _totalSizes {other._totalSizes}, _usedSizes {other._usedSizes} {
  other._handle = VK_NULL_HANDLE;
}

DescriptorPool::~DescriptorPool() {
  vkDestroyDescriptorPool(VulkanHandler::getDevice()->handle, _handle, nullptr);
}

void DescriptorPool::reset() const {
  vkResetDescriptorPool(VulkanHandler::getDevice()->handle, _handle, 0);
}
