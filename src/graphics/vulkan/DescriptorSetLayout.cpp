#include "DescriptorSetLayout.h"

#include "Device.h"

VkDescriptorSetLayoutCreateInfo DescriptorSetLayout::getCreateInfo(
  std::vector<VkDescriptorSetLayoutBinding> bindings) const {
  VkDescriptorSetLayoutCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  //info.pNext;
  //info.flags;
  info.bindingCount = (uint32_t) bindings.size();
  VkDescriptorSetLayoutBinding* bindingsPtr = new VkDescriptorSetLayoutBinding[info.bindingCount];
  std::copy(bindings.begin(), bindings.end(), bindingsPtr);
  info.pBindings = bindingsPtr;

  return info;
}

DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<Device> device,
  std::vector<VkDescriptorSetLayoutBinding> bindings) {
  _device = device;

  VkDescriptorSetLayoutCreateInfo createInfo = getCreateInfo(bindings);
  VK_CHECK(vkCreateDescriptorSetLayout(device->handle, &createInfo, nullptr, &handle));
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(_device->handle, handle, nullptr);
}
