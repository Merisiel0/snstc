#include "GraphicsPipelineId.h"

GraphicsPipelineId::GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode,
  MeshLayout layout, LightingType lighting) {
  _value |= ((uint16_t) topology) & TOPOLOGY_MASK << TOPOLOGY_POSITION;
  _value |= (((uint16_t) polygonMode) & MODE_MASK) << MODE_POSITION;
  _value |= (((uint16_t) layout) & LAYOUT_MASK) << LAYOUT_POSITION;
  _value |= (((uint16_t) lighting) & LIGHTING_MASK) << LIGHTING_POSITION;
}

GraphicsPipelineId::GraphicsPipelineId(UniqueGraphicsPipeline unique) {
  _value = (((uint16_t) unique) & 0b1) << 9;
}

void GraphicsPipelineId::setPrimitiveTopology(VkPrimitiveTopology topology) {
  _value &= TOPOLOGY_CLEAR;
  _value |= ((uint16_t) topology) & TOPOLOGY_MASK;
}

VkPrimitiveTopology GraphicsPipelineId::getPrimitiveTopology() const {
  return (VkPrimitiveTopology) (_value & TOPOLOGY_MASK);
}

void GraphicsPipelineId::setPolygonMode(VkPolygonMode topology) {
  _value &= MODE_CLEAR;
  _value |= ((uint16_t) topology) & MODE_MASK;
}

VkPolygonMode GraphicsPipelineId::getPolygonMode() const {
  return (VkPolygonMode) (_value & MODE_MASK);
}

void GraphicsPipelineId::setMeshLayout(MeshLayout layout) {
  _value &= LAYOUT_CLEAR;
  _value |= ((uint16_t) layout) & LAYOUT_MASK;
}

MeshLayout GraphicsPipelineId::getMeshLayout() const { return (MeshLayout) (_value & LAYOUT_MASK); }

void GraphicsPipelineId::setLightingType(LightingType type) {
  _value &= LIGHTING_CLEAR;
  _value |= ((uint16_t) type) & LIGHTING_MASK;
}

LightingType GraphicsPipelineId::getLightingType() const {
  return (LightingType) (_value & LIGHTING_MASK);
}

void GraphicsPipelineId::setCustom(uint16_t value) {
  _value &= CUSTOM_CLEAR;
  _value |= value & CUSTOM_MASK;
}

uint16_t GraphicsPipelineId::getCustom() { return _value & CUSTOM_MASK; }
