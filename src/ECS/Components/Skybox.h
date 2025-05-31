#pragma once

#include "BaseComponent.h"
#include "graphics/vulkan/GraphicsPipelineId.h"

#include <memory>
#include <string>
#include <vector>

class Mesh;
class Image;

struct Skybox : ECS::BaseComponent {
private:
  std::shared_ptr<Mesh> _mesh;
  std::shared_ptr<Image> _cubemap;

  GraphicsPipelineId _pipelineId;

public:
  Skybox(std::vector<std::string> cubemapFaces);

  std::shared_ptr<Mesh> getMesh() const;
  std::shared_ptr<Image> getCubeMap() const;
  GraphicsPipelineId getGraphicsPipelineId() const;
};