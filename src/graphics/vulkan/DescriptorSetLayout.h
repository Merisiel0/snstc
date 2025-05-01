#pragma once

#include "VulkanUtils.h"

class Device;

enum DescriptorSetLayoutType {
  DESCRIPTOR_SET_LAYOUT_SCENE,
  DESCRIPTOR_SET_LAYOUT_SKYBOX,
  DESCRIPTOR_SET_LAYOUT_OBJECT
};

class DescriptorSetLayout {
private:
  std::shared_ptr<Device> _device;

  VkDescriptorSetLayoutCreateInfo getCreateInfo(
    std::vector<VkDescriptorSetLayoutBinding>& bindings) const;

public:
  VkDescriptorSetLayout handle;
  DescriptorSetLayoutType type;

  DescriptorSetLayout(std::shared_ptr<Device> device,
    std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorSetLayoutType type);
  ~DescriptorSetLayout();
};