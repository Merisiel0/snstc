#pragma once

#include "_graphics/_vulkan/VulkanUtils.h"
#include "_resources/Mesh.h"
#include "BaseComponent.h"

class Material;
struct PushConstants;

struct MeshProperties {
  mat4 transform;
};

struct MeshRenderer : public ECS::BaseComponent {
  Mesh* mesh;
  Material* material;
  VkCullModeFlags cullMode;
  VkPolygonMode polygonMode;
  float lineWidth = 1.0f;
  std::vector<mat4> instanceTransforms;

  MeshRenderer(Mesh* mesh, Material* material) : mesh{ mesh }, material{ material }, cullMode{ VK_CULL_MODE_BACK_BIT },
    polygonMode{ VK_POLYGON_MODE_FILL } {
  };

  MeshRenderer(Mesh* mesh, Material* material, VkCullModeFlags cullMode) : mesh{ mesh }, material{ material },
    cullMode{ cullMode }, polygonMode{ VK_POLYGON_MODE_FILL } {
  };

  MeshRenderer(Mesh* mesh, Material* material, VkPolygonMode polygonMode) : mesh{ mesh }, material{ material },
    cullMode{ VK_CULL_MODE_BACK_BIT }, polygonMode{ polygonMode } {
  };

  MeshRenderer(Mesh* mesh, Material* material, VkCullModeFlags cullMode, VkPolygonMode polygonMode) : mesh{ mesh }, material{ material },
    cullMode{ cullMode }, polygonMode{ polygonMode } {
  };

  MeshRenderer(Mesh* mesh, Material* material, VkCullModeFlags cullMode, VkPolygonMode polygonMode,
    float lineWidth, std::vector<mat4> instanceTransforms)
    : mesh{ mesh }, material{ material }, cullMode{ cullMode }, polygonMode{ polygonMode },
    lineWidth{ lineWidth }, instanceTransforms{ instanceTransforms } {
  };

  MeshRenderer(const MeshRenderer& mr) : MeshRenderer(mr.mesh, mr.material, mr.cullMode, mr.polygonMode, mr.lineWidth, mr.instanceTransforms) {};

  ~MeshRenderer();
};