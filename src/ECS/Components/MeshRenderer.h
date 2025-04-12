#pragma once

#include "BaseComponent.h"
#include "graphics/vulkan/VulkanUtils.h"
#include "resources/Mesh.h"

class Material;
struct PushConstants;

struct MeshProperties {
  mat4 transform;
};

struct MeshRenderer : public ECS::BaseComponent {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  VkCullModeFlags cullMode;
  VkPolygonMode polygonMode;
  float lineWidth = 1.0f;
  std::vector<mat4> instanceTransforms;

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
      mesh {mesh},
      material {material},
      cullMode {VK_CULL_MODE_BACK_BIT},
      polygonMode {VK_POLYGON_MODE_FILL} {};

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    VkCullModeFlags cullMode) :
      mesh {mesh}, material {material}, cullMode {cullMode}, polygonMode {VK_POLYGON_MODE_FILL} {};

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    VkPolygonMode polygonMode) :
      mesh {mesh},
      material {material},
      cullMode {VK_CULL_MODE_BACK_BIT},
      polygonMode {polygonMode} {};

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    VkCullModeFlags cullMode, VkPolygonMode polygonMode) :
      mesh {mesh}, material {material}, cullMode {cullMode}, polygonMode {polygonMode} {};

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    VkCullModeFlags cullMode, VkPolygonMode polygonMode, float lineWidth,
    std::vector<mat4> instanceTransforms) :
      mesh {mesh},
      material {material},
      cullMode {cullMode},
      polygonMode {polygonMode},
      lineWidth {lineWidth},
      instanceTransforms {instanceTransforms} {};

  MeshRenderer(const MeshRenderer& mr) :
      MeshRenderer(mr.mesh, mr.material, mr.cullMode, mr.polygonMode, mr.lineWidth,
        mr.instanceTransforms) {}

  MeshRenderer(MeshRenderer&& mr) noexcept :
      mesh {mr.mesh},
      material {mr.material},
      cullMode {mr.cullMode},
      polygonMode {mr.polygonMode},
      lineWidth {mr.lineWidth},
      instanceTransforms {std::move(mr.instanceTransforms)} {
    mr.mesh = nullptr;
    mr.material = nullptr;
  }

  ~MeshRenderer();
};