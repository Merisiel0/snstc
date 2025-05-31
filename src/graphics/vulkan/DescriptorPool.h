#pragma once

#include "VulkanUtils.h"

class DescriptorSetLayout;

class DescriptorPool {
private:
  VkDescriptorPool _handle;
  std::vector<VkDescriptorPoolSize> _totalSizes;
  std::vector<VkDescriptorPoolSize> _usedSizes;

  VkDescriptorPoolCreateInfo getCreateInfo() const;

public: 
  /// @brief Gets the vulkan handle of this object.
  /// @return a `VkDescriptorPool` handle.
  VkDescriptorPool getHandle() const;

  /// @brief Gets how many `VkDescriptorPoolSize` this pool can contain.
  /// @return the total sizes.
  std::vector<VkDescriptorPoolSize> getTotalSizes() const;

  /// @brief Gets how many `VkDescriptorPoolSize` this pool currently contains.
  /// @return the used sizes.
  std::vector<VkDescriptorPoolSize> getUsedSizes() const;

  /// @brief Gets how many more `VkDescriptorPoolSize` this pool can contain.
  /// @return the free sizes.
  std::vector<VkDescriptorPoolSize> getFreeSizes() const;

  /// @brief Adds `VkDescriptorPoolSize` to this pool.
  /// @param sizes sizes to add.
  void addSizes(std::vector<VkDescriptorPoolSize> sizes);

  /// @brief Removes `VkDescriptorPoolSize` from this pool.
  /// @param sizes sizes to remove.
  void removeSizes(std::vector<VkDescriptorPoolSize> sizes);

  DescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes);
  ~DescriptorPool();

  DescriptorPool(const DescriptorPool&) = delete;
  DescriptorPool& operator=(const DescriptorPool&) = delete;

  DescriptorPool(DescriptorPool&& other);

  /// @brief Resets this pool.
  void reset() const;
};