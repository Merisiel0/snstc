#pragma once

#include "VulkanUtils.h"

enum DescriptorSetLayoutType : uint8_t;

class DescriptorSetLayout {
private:
  VkDescriptorSetLayout _handle;

  DescriptorSetLayoutType _type;
  std::vector<VkDescriptorPoolSize> _poolSizes;

  VkDescriptorSetLayoutCreateInfo getCreateInfo(
    std::vector<VkDescriptorSetLayoutBinding>& bindings) const;

public:
  VkDescriptorSetLayout getHandle() const;

  DescriptorSetLayoutType getType() const;
  std::vector<VkDescriptorPoolSize> getPoolSizes() const;

  DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings,
    DescriptorSetLayoutType type);
  ~DescriptorSetLayout();
};