#pragma once

#include "DescriptorManager.h"
#include "VulkanUtils.h"

class GraphicsPipeline;
class Shader;
class PipelineLayout;

/// @brief Manager class to create graphics pipelines and layouts
class GraphicsPipelineManager {
private:
  static std::vector<std::weak_ptr<PipelineLayout>> _layouts;
  static std::vector<std::weak_ptr<GraphicsPipeline>> _pipelines;

public:
  /// @brief Creates a new graphics pipeline with the given parameters if it exists, returns it otherwise.
  /// @param shaders a vector of shaders.
  /// @param setLayouts a vector of descriptor set layout types.
  /// @param pushConstantRanges a vector of push constant ranges.
  /// @param primitiveTopology a primitive topology.
  /// @param polygonMode a polygon mode.
  /// @param depthWrite wether to depth write or not.
  /// @return a shared pointer to the new graphics pipeline.
  static std::shared_ptr<GraphicsPipeline> createPipeline(
    std::vector<std::shared_ptr<Shader>> shaders,
    std::vector<DescriptorSetLayoutType> setLayouts,
    std::vector<VkPushConstantRange> pushConstantRanges,
    VkPrimitiveTopology primitiveTopology,
    VkPolygonMode polygonMode,
    bool depthWrite);

  /// @brief Gets all pipeline layouts that contains a specific descriptor set layout type.
  /// @param setLayout a descriptor set layout type.
  /// @return a vector of shared pointer to pipeline layouts.
  static std::vector<std::shared_ptr<PipelineLayout>> getLayoutWithSet(
    DescriptorSetLayoutType setLayout);

  /// @brief Frees all expired pipeline layouts.
  ///
  /// This method is called internally when layouts are destroyed.
  ///
  /// Calling it again is useless.
  static void cleanupExpiredLayouts();

  /// @brief Frees all expired graphics pipelines.
  ///
  /// This method is called internally when pipelines are destroyed.
  ///
  /// Calling it again is useless.
  static void cleanupExpired();

  /// @brief Frees all graphics pipelines and layouts.
  static void cleanup();
};