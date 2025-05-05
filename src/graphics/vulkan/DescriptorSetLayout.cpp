#include "DescriptorSetLayout.h"

#include "Device.h"

VkDescriptorSetLayoutCreateInfo DescriptorSetLayout::getCreateInfo(
  std::vector<VkDescriptorSetLayoutBinding>& bindings) const {
  VkDescriptorSetLayoutCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.bindingCount = (uint32_t) bindings.size();
  info.pBindings = bindings.data();

  return info;
}

VkDescriptorSetLayout DescriptorSetLayout::getHandle() const { return _handle; }

DescriptorSetLayoutType DescriptorSetLayout::getType() const { return _type; }

std::vector<VkDescriptorPoolSize> DescriptorSetLayout::getPoolSizes() const { return _poolSizes; }

DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<Device> device,
  std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorSetLayoutType type) :
    _device {device}, _type {type} {
  for(const auto& binding : bindings) {
    const auto it = std::find_if(_poolSizes.begin(), _poolSizes.end(),
      [binding](VkDescriptorPoolSize size) { return size.type = binding.descriptorType; });

    if(it != _poolSizes.end()) {
      it->descriptorCount += binding.descriptorCount;
    } else {
      VkDescriptorPoolSize size;
      size.type = binding.descriptorType;
      size.descriptorCount = binding.descriptorCount;
      _poolSizes.push_back(size);
    }
  }

  VkDescriptorSetLayoutCreateInfo createInfo = getCreateInfo(bindings);
  VK_CHECK(vkCreateDescriptorSetLayout(device->handle, &createInfo, nullptr, &_handle));
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(_device->handle, _handle, nullptr);
}
