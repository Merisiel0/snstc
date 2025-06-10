#pragma once

#include "BaseComponent.h"
#include "graphics/vulkan/Swapchain.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

class Mesh;
class Image;
class GraphicsPipeline;

struct Skybox : ECS::BaseComponent {
private:
  const std::shared_ptr<Mesh> _mesh;
  std::shared_ptr<Image> _cubemap;
  std::array<std::shared_ptr<DescriptorSet>, Swapchain::FRAME_OVERLAP> _descriptorSets;
  std::shared_ptr<GraphicsPipeline> _pipeline;

public:
  Skybox(std::vector<std::string> cubemapFaces);

  std::shared_ptr<Mesh> getMesh() const;
  std::shared_ptr<Image> getCubeMap() const;
  const GraphicsPipeline& getPipeline() const;
};