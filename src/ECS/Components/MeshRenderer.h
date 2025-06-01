#pragma once

#include "BaseComponent.h"
#include "graphics/vulkan/GraphicsPipelineId.h"
#include "graphics/vulkan/VulkanUtils.h"
#include "resources/Mesh.h"

class Material;
class CommandBuffer;
class Buffer;
struct PushConstants;
class Shader;
class DescriptorSetLayout;
enum DescriptorSetLayoutType : uint8_t;

struct MeshRenderer : public ECS::BaseComponent {
private:
  GraphicsPipelineId _pipelineId;
  VkPolygonMode _polygonMode;
  VkPrimitiveTopology _topology;
  LightingType _lightingType;

public:
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  VkCullModeFlags cullMode;
  float lineWidth = 1.0f;

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
    VkPrimitiveTopology topology, VkCullModeFlags cullMode, VkPolygonMode polygonMode,
    LightingType lightingType);

  MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

  MeshRenderer(std::shared_ptr<Mesh> mesh, VkCullModeFlags cullMode, CustomGraphicsPipeline id,
    VkPrimitiveTopology topology, VkPolygonMode polygonMode,
    std::vector<VkPushConstantRange> pushConstantRanges, const std::vector<Shader>& shaders,
    std::vector<DescriptorSetLayoutType> setLayouts);

  MeshRenderer(MeshRenderer&&) noexcept = default;
  MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

  ~MeshRenderer();

  GraphicsPipelineId getGraphicsPipelineId() const;

  VkPolygonMode getPolygonMode() const;
  void setPolygonMode(VkPolygonMode mode);

  VkPrimitiveTopology getPrimitiveTopology() const;
  void setPrimitiveTopology(VkPrimitiveTopology topology);

  LightingType getLightingType() const;
  void setLightingType(LightingType type);
};