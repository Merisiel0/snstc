#include "GraphicsPipelineId.h"

GraphicsPipelineId::GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode,
  MeshLayout layout, LightingType lighting) {
  _value = 0;
  _value |= ((uint32_t) topology) & TOPOLOGY_MASK << TOPOLOGY_POSITION;
  _value |= (((uint32_t) polygonMode) & MODE_MASK) << MODE_POSITION;
  _value |= (((uint32_t) layout) & LAYOUT_MASK) << LAYOUT_POSITION;
  _value |= (((uint32_t) lighting) & LIGHTING_MASK) << LIGHTING_POSITION;
}

GraphicsPipelineId::GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode,
  MeshLayout layout, EngineGraphicsPipeline engine) {
  _value = 0;
  _value |= ((uint32_t) topology) & TOPOLOGY_MASK << TOPOLOGY_POSITION;
  _value |= (((uint32_t) polygonMode) & MODE_MASK) << MODE_POSITION;
  _value |= (((uint32_t) layout) & LAYOUT_MASK) << LAYOUT_POSITION;
  _value |= (((uint32_t) engine) & ENGINE_MASK) << ENGINE_POSITION;
}

GraphicsPipelineId::GraphicsPipelineId(VkPrimitiveTopology topology, VkPolygonMode polygonMode,
  MeshLayout layout, CustomGraphicsPipeline custom) {
  _value = 0;
  _value |= ((uint32_t) topology) & TOPOLOGY_MASK << TOPOLOGY_POSITION;
  _value |= (((uint32_t) polygonMode) & MODE_MASK) << MODE_POSITION;
  _value |= (((uint32_t) layout) & LAYOUT_MASK) << LAYOUT_POSITION;
  _value |= (((uint32_t) custom) & CUSTOM_MASK) << CUSTOM_POSITION;
}

void GraphicsPipelineId::setPrimitiveTopology(VkPrimitiveTopology topology) {
  _value &= TOPOLOGY_CLEAR;
  _value |= ((uint32_t) topology) & TOPOLOGY_MASK << TOPOLOGY_POSITION;
}

VkPrimitiveTopology GraphicsPipelineId::getPrimitiveTopology() const {
  return (VkPrimitiveTopology) ((_value >> TOPOLOGY_POSITION) & TOPOLOGY_MASK);
}

void GraphicsPipelineId::setPolygonMode(VkPolygonMode mode) {
  _value &= MODE_CLEAR;
  _value |= (((uint32_t) mode) & MODE_MASK) << MODE_POSITION;
}

VkPolygonMode GraphicsPipelineId::getPolygonMode() const {
  return (VkPolygonMode) ((_value >> MODE_POSITION) & MODE_MASK);
}

void GraphicsPipelineId::setMeshLayout(MeshLayout layout) {
  _value &= LAYOUT_CLEAR;
  _value |= (((uint32_t) layout) & LAYOUT_MASK) << LAYOUT_POSITION;
}

MeshLayout GraphicsPipelineId::getMeshLayout() const {
  return (MeshLayout) ((_value >> LAYOUT_POSITION) & LAYOUT_MASK);
}

void GraphicsPipelineId::setLightingType(LightingType type) {
  _value &= LIGHTING_CLEAR;
  _value |= (((uint32_t) type) & LIGHTING_MASK) << LIGHTING_POSITION;
}

LightingType GraphicsPipelineId::getLightingType() const {
  return (LightingType) ((_value >> LIGHTING_POSITION) & LIGHTING_MASK);
}

void GraphicsPipelineId::setEngine(EngineGraphicsPipeline value) {
  _value &= ENGINE_CLEAR;
  _value |= (((uint32_t) value) & ENGINE_MASK) << ENGINE_POSITION;
}

EngineGraphicsPipeline GraphicsPipelineId::getEngine() const {
  return (EngineGraphicsPipeline) ((_value >> ENGINE_POSITION) & ENGINE_MASK);
}

void GraphicsPipelineId::setCustom(CustomGraphicsPipeline value) {
  _value &= CUSTOM_CLEAR;
  _value |= (((uint32_t) value) & CUSTOM_MASK) << CUSTOM_POSITION;
}

CustomGraphicsPipeline GraphicsPipelineId::getCustom() const {
  return (CustomGraphicsPipeline) ((_value >> CUSTOM_POSITION) & CUSTOM_MASK);
}

bool GraphicsPipelineId::operator==(const GraphicsPipelineId& other) const {
  return _value == other._value;
}

bool GraphicsPipelineId::operator!=(const GraphicsPipelineId& other) const {
  return _value != other._value;
}

GraphicsPipelineId::operator uint32_t() const { return _value; }
