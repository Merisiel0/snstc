#include "MeshRenderer.h"

#include "ECS/Components/Transform.h"
#include "ECS/GameObject.h"
#include "graphics/vulkan/Buffer.h"
#include "graphics/vulkan/GraphicsPipelineId.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "resources/Material.h"

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
  VkPrimitiveTopology topology, VkCullModeFlags cullMode, VkPolygonMode polygonMode,
  LightingType lightingType) :
    mesh {mesh},
    material {material},
    _topology {topology},
    cullMode {cullMode},
    _polygonMode {polygonMode},
    _lightingType {lightingType},
    _pipelineId {GraphicsPipelineId(topology, polygonMode, MESH_LAYOUT_STATIC, lightingType)} {
  VulkanHandler::loadPipeline(_pipelineId);
}

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    MeshRenderer(mesh, material, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_BACK_BIT,
      VK_POLYGON_MODE_FILL, LIGHTING_TYPE_UNLIT) {}

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, VkCullModeFlags cullMode,
  CustomGraphicsPipeline id, VkPrimitiveTopology topology, VkPolygonMode polygonMode,
  std::vector<VkPushConstantRange> pushConstantRanges, std::vector<Shader> shaders,
  std::vector<DescriptorSetLayoutType> setLayouts) :
    mesh {mesh},
    material {nullptr},
    _topology {topology},
    cullMode {cullMode},
    _polygonMode {polygonMode},
    _lightingType {LIGHTING_TYPE_UNDEFINED},
    _pipelineId {GraphicsPipelineId(topology, polygonMode, MESH_LAYOUT_STATIC, id)} {
  VulkanHandler::loadPipeline(_pipelineId, shaders, pushConstantRanges, setLayouts);
}

MeshRenderer::~MeshRenderer() {}

GraphicsPipelineId MeshRenderer::getGraphicsPipelineId() const { return _pipelineId; }

VkPolygonMode MeshRenderer::getPolygonMode() const { return _polygonMode; }

void MeshRenderer::setPolygonMode(VkPolygonMode mode) {
  _polygonMode = mode;
  _pipelineId.setPolygonMode(mode);
  VulkanHandler::loadPipeline(_pipelineId);
}

VkPrimitiveTopology MeshRenderer::getPrimitiveTopology() const { return _topology; }

void MeshRenderer::setPrimitiveTopology(VkPrimitiveTopology topology) {
  _topology = topology;
  _pipelineId.setPrimitiveTopology(topology);
  VulkanHandler::loadPipeline(_pipelineId);
}

LightingType MeshRenderer::getLightingType() const { return _lightingType; }

void MeshRenderer::setLightingType(LightingType type) {
  _lightingType = type;
  _pipelineId.setLightingType(type);
  VulkanHandler::loadPipeline(_pipelineId);
}
