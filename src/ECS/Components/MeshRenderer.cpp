#include "MeshRenderer.h"

#include "graphics/vulkan/GraphicsPipeline.h"
#include "ECS/GameObject.h"
#include "ECS/Components/Transform.h"
#include "graphics/vulkan/Buffer.h"
#include "resources/Material.h"

MeshRenderer::~MeshRenderer() {
  delete mesh;
  delete material;
}
