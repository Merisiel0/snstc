#pragma once

#include "DescriptorManager.h"
#include "VulkanUtils.h"
#include "resources/Resource.h"
#include "utils/Hashing.h"

struct PipelineLayoutSettings {
private:
  const std::vector<DescriptorSetLayoutType> _setLayouts;
  const std::vector<VkPushConstantRange> _pushConstantRanges;

public:
  PipelineLayoutSettings() {};

  PipelineLayoutSettings(std::vector<DescriptorSetLayoutType> _setLayouts,
    std::vector<VkPushConstantRange> _pushConstantRanges);

  std::vector<DescriptorSetLayoutType> getSetLayouts() const;
  std::vector<VkPushConstantRange> getPushConstantRanges() const;

  bool operator==(const PipelineLayoutSettings& other) const;
};

class PipelineLayout : public Resource<PipelineLayout, PipelineLayoutSettings> {
private:
  VkPipelineLayout _handle;
  const PipelineLayoutSettings _settings;

  VkPipelineLayoutCreateInfo getCreateInfo() const;

  PipelineLayout(PipelineLayoutSettings settings);

public:
  /// @brief Gets the handle to this pipeline layout.
  /// @return a pipeline layout handle.
  const VkPipelineLayout& getHandle() const;

  static std::shared_ptr<PipelineLayout> load(
    const std::vector<DescriptorSetLayoutType>& setLayouts,
    const std::vector<VkPushConstantRange>& pushConstantRanges);

  ~PipelineLayout();

  /// @brief Gets this pipeline layout's settings.
  /// @return a `PipelineLayoutSettings` structure.
  const PipelineLayoutSettings& getSettings() const;

  /// @brief Checks if this pipeline layout's settings matches the provided settings.
  /// @param settings settings to check for matching.
  /// @return true if the settings match, false otherwise.
  bool matchSettings(PipelineLayoutSettings settings) const;

  /// @brief Checks if this pipeline layout supports a descriptor set layout.
  /// @param layout a descriptor set layout type.
  /// @return true if the descriptor set layout is supported, false otherwise.
  bool hasSetLayout(DescriptorSetLayoutType layout) const;

  bool operator==(const PipelineLayout& other);
};

namespace std {
  template<>
  struct hash<PipelineLayoutSettings> {
    std::size_t operator()(const PipelineLayoutSettings& s) const {
      std::size_t seed = 0;
      hashCombine(seed, hashVectorUnordered(s.getSetLayouts()));
      hashCombine(seed, hashVectorUnordered(s.getPushConstantRanges()));
      return seed;
    }
  };
} // namespace std
