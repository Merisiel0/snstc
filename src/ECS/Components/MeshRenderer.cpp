#include "MeshRenderer.h"

#include "ECS/Components/Transform.h"
#include "ECS/GameObject.h"
#include "graphics/vulkan/Buffer.h"
#include "graphics/vulkan/GraphicsPipeline.h"
#include "resources/Material.h"

void MeshRenderer::updatePipelineId() {
  _pipelineId = createPipelineId(_topology, _polygonMode, MESH_LAYOUT_STATIC, _lightingType);
}

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
  VkPrimitiveTopology topology, VkCullModeFlags cullMode, VkPolygonMode polygonMode,
  LightingType lightingType) :
    mesh {mesh},
    material {material},
    _topology {topology},
    cullMode {cullMode},
    _polygonMode {polygonMode},
    _lightingType {lightingType} {
  updatePipelineId();
}

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    MeshRenderer(mesh, material, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_BACK_BIT,
      VK_POLYGON_MODE_FILL, LIGHTING_TYPE_UNLIT) {}

MeshRenderer::~MeshRenderer() {}

GraphicsPipelineId MeshRenderer::getGraphicsPipelineId() const { return _pipelineId; }

VkPolygonMode MeshRenderer::getPolygonMode() const { return _polygonMode; }

void MeshRenderer::setPolygonMode(VkPolygonMode mode) {
  _polygonMode = mode;
  updatePipelineId();
}

VkPrimitiveTopology MeshRenderer::getPrimitiveTopology() const { return _topology; }

void MeshRenderer::setPrimitiveTopology(VkPrimitiveTopology topology) {
  _topology = topology;
  updatePipelineId();
}

LightingType MeshRenderer::getLightingType() const { return _lightingType; }

void MeshRenderer::setLightingType(LightingType type) {
  _lightingType = type;
  updatePipelineId();
}
