#pragma once

#include "BaseComponent.h"
#include "graphics/vulkan/VulkanUtils.h"
#include "resources/Mesh.h"

class Material;
class CommandBuffer;
class Buffer;
struct PushConstants;

struct InstanceProperties {
  mat4 transform;
};

struct MeshRenderer : public ECS::BaseComponent {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  VkCullModeFlags cullMode;
  VkPolygonMode polygonMode;
  float lineWidth = 1.0f;

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

  MeshRenderer(MeshRenderer&&) noexcept = default;
  MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

  ~MeshRenderer();
};