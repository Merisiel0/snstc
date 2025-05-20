#pragma once

#include "VulkanUtils.h"

/// @brief A representation of a graphics pipeline's id.
class GraphicsPipelineId {
private:
  static const uint16_t TOPOLOGY_POSITION = 0;
  static const uint16_t TOPOLOGY_MASK = 0b111;
  static const uint16_t TOPOLOGY_CLEAR = 0xFFF8;

  static const uint16_t MODE_POSITION = 3;
  static const uint16_t MODE_MASK = 0b11;
  static const uint16_t MODE_CLEAR = 0xFFE7;

  static const uint16_t LAYOUT_POSITION = 5;
  static const uint16_t LAYOUT_MASK = 0b11;
  static const uint16_t LAYOUT_CLEAR = 0xFF9F;

  static const uint16_t LIGHTING_POSITION = 7;
  static const uint16_t LIGHTING_MASK = 0b11;
  static const uint16_t LIGHTING_CLEAR = 0xFE7F;

  static const uint16_t CUSTOM_POSITION = 9;
  static const uint16_t CUSTOM_MASK = 0b1111111;
  static const uint16_t CUSTOM_CLEAR = 0x1FF;

  uint16_t _value;

public:
  /// @brief Converts a mesh's parameters into a graphics pipeline id.
  /// @param topology a primitive topology to encode in bits 0-2
  /// @param polygonMode a polygon mode to encode in bits 3-4.
  /// @param layout a mesh layout to encode in bits 5-6.
  /// @param lighting a lighting type to encode in bits 7-8.
  GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode, MeshLayout layout,
    LightingType lighting);

  /// @brief Makes a unique graphics pipeline id.
  ///
  /// TODO: implement custom pipelines and change this to use them.
  /// @param unique a unique graphics pipeline.
  GraphicsPipelineId(UniqueGraphicsPipeline unique);

  void setPrimitiveTopology(VkPrimitiveTopology topology);
  VkPrimitiveTopology getPrimitiveTopology() const;

  void setPolygonMode(VkPolygonMode mode);
  VkPolygonMode getPolygonMode() const;

  void setMeshLayout(MeshLayout layout);
  MeshLayout getMeshLayout() const;

  void setLightingType(LightingType type);
  LightingType getLightingType() const;

  void setCustom(uint16_t value);
  uint16_t getCustom();

  bool operator==(const GraphicsPipelineId& other) const;
  bool operator!=(const GraphicsPipelineId& other) const;

  operator uint16_t() const;
};

namespace std {
  template<>
  struct hash<GraphicsPipelineId> {
    size_t operator()(const GraphicsPipelineId& id) const noexcept {
      return std::hash<uint16_t>()(static_cast<uint16_t>(id));
    }
  };
} // namespace std