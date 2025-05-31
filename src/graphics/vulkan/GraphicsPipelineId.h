#pragma once

#include "VulkanUtils.h"

enum CustomGraphicsPipeline : uint16_t;

enum EngineGraphicsPipeline : uint32_t { ENGINE_PIPELINE_ID_SKYBOX };

/// @brief A representation of a graphics pipeline's id.
///
/// Encoded information:
///
/// custom, lighting, layout, mode, topology
///
/// [ 15-9 | 8 7 | 6 5 | 4 3 | 2 1 0 ]
/// TODO: merge lighting and custom bits
class GraphicsPipelineId {
private:
  /// @brief topology encoding starts at bit 0
  static const uint32_t TOPOLOGY_POSITION = 0;
  /// @brief topology encoding is 3 bits long
  static const uint32_t TOPOLOGY_MASK = 0b111;
  /// @brief topology is encoded in bits 0-1-2
  static const uint32_t TOPOLOGY_CLEAR = 0xFFF8;

  static const uint32_t MODE_POSITION = 3;
  static const uint32_t MODE_MASK = 0b11;
  static const uint32_t MODE_CLEAR = 0xFFE7;

  static const uint32_t LAYOUT_POSITION = 5;
  static const uint32_t LAYOUT_MASK = 0b11;
  static const uint32_t LAYOUT_CLEAR = 0xFF9F;

  static const uint32_t LIGHTING_POSITION = 7;
  static const uint32_t LIGHTING_MASK = 0b11;
  static const uint32_t LIGHTING_CLEAR = 0xFE7F;

  static const uint32_t ENGINE_POSITION = 9;
  static const uint32_t ENGINE_MASK = 0b1;
  static const uint32_t ENGINE_CLEAR = 0xFDFF;

  static const uint32_t CUSTOM_POSITION = 10;
  static const uint32_t CUSTOM_MASK = 0b111111;
  static const uint32_t CUSTOM_CLEAR = 0x3FF;

  uint32_t _value;

public:
  /// @brief Converts a mesh's parameters into a graphics pipeline id.
  /// @param topology a primitive topology to encode in bits 0-2
  /// @param polygonMode a polygon mode to encode in bits 3-4.
  /// @param layout a mesh layout to encode in bits 5-6.
  /// @param lighting a lighting type to encode in bits 7-8.
  GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode, MeshLayout layout,
    LightingType lighting);

  /// @brief Converts a mesh's parameters into a graphics pipeline id.
  /// @param topology a primitive topology to encode in bits 0-2
  /// @param polygonMode a polygon mode to encode in bits 3-4.
  /// @param layout a mesh layout to encode in bits 5-6.
  /// @param engine an engine pipeline id to encode in bit 9.
  GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode, MeshLayout layout,
    EngineGraphicsPipeline engine);

  /// @brief Converts a mesh's parameters into a graphics pipeline id.
  /// @param topology a primitive topology to encode in bits 0-2
  /// @param polygonMode a polygon mode to encode in bits 3-4.
  /// @param layout a mesh layout to encode in bits 5-6.
  /// @param custom a custom type to encode in bits 10-15.
  GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode, MeshLayout layout,
    CustomGraphicsPipeline custom);

  void setPrimitiveTopology(VkPrimitiveTopology topology);
  VkPrimitiveTopology getPrimitiveTopology() const;

  void setPolygonMode(VkPolygonMode mode);
  VkPolygonMode getPolygonMode() const;

  void setMeshLayout(MeshLayout layout);
  MeshLayout getMeshLayout() const;

  void setLightingType(LightingType type);
  LightingType getLightingType() const;

  void setEngine(EngineGraphicsPipeline value);
  EngineGraphicsPipeline getEngine() const;

  void setCustom(CustomGraphicsPipeline value);
  CustomGraphicsPipeline getCustom() const;

  bool operator==(const GraphicsPipelineId& other) const;
  bool operator!=(const GraphicsPipelineId& other) const;

  operator uint32_t() const;
};

namespace std {
  template<>
  struct hash<GraphicsPipelineId> {
    size_t operator()(const GraphicsPipelineId& id) const noexcept {
      return std::hash<uint32_t>()(static_cast<uint32_t>(id));
    }
  };
} // namespace std