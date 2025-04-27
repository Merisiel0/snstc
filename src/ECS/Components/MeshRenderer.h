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
private:
  std::shared_ptr<Buffer> _instanceBuffer;
  std::vector<InstanceProperties> _instances;
  int _maxInstances = 0;

public:
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  VkCullModeFlags cullMode;
  VkPolygonMode polygonMode;
  float lineWidth = 1.0f;

  /// @brief Updates the instance buffer with the current instances.
  /// @param commandBuffer a command buffer.
  void updateInstanceBuffer(std::shared_ptr<CommandBuffer> commandBuffer);

  /// @brief Gets the GPU memory containing the instances.
  /// @param commandBuffer a command buffer.
  /// @return A buffer containing an array of `InstanceProperties` on GPU memory.
  std::shared_ptr<Buffer> getInstancesBuffer();

  /// @brief Gets the maximum count of instances.
  /// @return the maximum count of instances.
  int getMaxInstanceCount();

  /// @brief Sets the maximum count of instances.
  ///
  /// This count, times the size of `InstanceProperties`, is the size reserved on GPU memory.
  ///
  /// Keep it to a minimum.
  /// @param max maximum count of instances.
  void setMaxInstanceCount(int max);

  /// @brief Gets how many instances this `MeshRenderer` currently has.
  /// @return count of instances.
  int getInstanceCount();

  /// @brief Adds an instance if there's place in the buffer.
  /// @param instance the instance to add.
  void addInstance(InstanceProperties instance);

  /// @brief Clears all instances from the buffer.
  ///
  /// This does not clear the memory, call `setMaxInstanceCount(0)` for that.
  void clearInstances();

  bool isInstancing() { return _maxInstances != 0; }

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
    std::vector<InstanceProperties> instances) :
      mesh {mesh},
      material {material},
      cullMode {cullMode},
      polygonMode {polygonMode},
      lineWidth {lineWidth},
      _instances {instances} {};
  
  MeshRenderer(MeshRenderer&&) noexcept = default;
  MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

  ~MeshRenderer();
};