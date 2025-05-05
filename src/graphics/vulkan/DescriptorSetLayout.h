#pragma once

#include "VulkanUtils.h"

class Device;

enum DescriptorSetLayoutType : uint8_t;

class DescriptorSetLayout {
private:
  std::shared_ptr<Device> _device;

  VkDescriptorSetLayout _handle;

  DescriptorSetLayoutType _type;
  std::vector<VkDescriptorPoolSize> _poolSizes;

  VkDescriptorSetLayoutCreateInfo getCreateInfo(
    std::vector<VkDescriptorSetLayoutBinding>& bindings) const;

public:
  VkDescriptorSetLayout getHandle() const;

  DescriptorSetLayoutType getType() const;
  std::vector<VkDescriptorPoolSize> getPoolSizes() const;

  DescriptorSetLayout(std::shared_ptr<Device> device,
    std::vector<VkDescriptorSetLayoutBinding>& bindings, DescriptorSetLayoutType type);
  ~DescriptorSetLayout();
};