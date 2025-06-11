#include "MeshRenderer.h"

#include "ECS/Components/Transform.h"
#include "ECS/GameObject.h"
#include "graphics/vulkan/Buffer.h"
#include "graphics/vulkan/GraphicsPipeline.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "resources/Material.h"
#include "resources/Mesh.h"

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh,
  std::shared_ptr<Material> material,
  std::shared_ptr<GraphicsPipeline> pipeline) :
    _mesh {mesh},
    _material {material},
    _pipeline {pipeline},
    _polygonMode {pipeline->getSettings().getPolygonMode()} {}

MeshRenderer::~MeshRenderer() {}

const Mesh& MeshRenderer::getMesh() const { return *_mesh; }

void MeshRenderer::setMesh(std::shared_ptr<Mesh> mesh) {
  if(mesh->getPrimitiveTopology() != _mesh->getPrimitiveTopology()) { _dirtyPipeline = true; }
  _mesh = mesh;
}

const Material& MeshRenderer::getMaterial() const { return *_material; }

void MeshRenderer::setMaterial(std::shared_ptr<Material> material) {
  if(material->hasTextures() != _material->hasTextures()) { _dirtyPipeline = true; }
  _material = material;
}

VkPolygonMode MeshRenderer::getPolygonMode() const { return _polygonMode; }

void MeshRenderer::setPolygonMode(VkPolygonMode mode) {
  if(mode != _polygonMode) {
    _polygonMode = mode;
    _dirtyPipeline = true;
  }
}

VkCullModeFlags MeshRenderer::getCullMode() const { return _cullMode; }

void MeshRenderer::setCullMode(VkCullModeFlags mode) { _cullMode = mode; }

float MeshRenderer::getLineWidth() const { return _lineWidth; }

void MeshRenderer::setLineWidth(float width) { _lineWidth = width; }
