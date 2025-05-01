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

DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<Device> device,
  std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorSetLayoutType type) :
    _device {device}, type {type} {
  VkDescriptorSetLayoutCreateInfo createInfo = getCreateInfo(bindings);
  VK_CHECK(vkCreateDescriptorSetLayout(device->handle, &createInfo, nullptr, &handle));
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(_device->handle, handle, nullptr);
}
