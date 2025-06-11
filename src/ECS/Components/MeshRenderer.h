#pragma once

#include "BaseComponent.h"
#include "graphics/vulkan/VulkanUtils.h"

class Mesh;
class Material;
class GraphicsPipeline;

struct MeshRenderer : public ECS::BaseComponent {
private:
  std::shared_ptr<Mesh> _mesh;
  std::shared_ptr<Material> _material;
  std::shared_ptr<GraphicsPipeline> _pipeline;
  bool _dirtyPipeline;
  VkPolygonMode _polygonMode;
  VkCullModeFlags _cullMode;
  float _lineWidth = 1.0f;

public:
  MeshRenderer(std::shared_ptr<Mesh> mesh,
    std::shared_ptr<Material> material,
    std::shared_ptr<GraphicsPipeline> pipeline);

  ~MeshRenderer();

  const Mesh& getMesh() const;
  void setMesh(std::shared_ptr<Mesh> mesh);

  const Material& getMaterial() const;
  void setMaterial(std::shared_ptr<Material> material);

  VkPolygonMode getPolygonMode() const;
  void setPolygonMode(VkPolygonMode mode);

  VkCullModeFlags getCullMode() const;
  void setCullMode(VkCullModeFlags mode);

  float getLineWidth() const;
  void setLineWidth(float width);
};