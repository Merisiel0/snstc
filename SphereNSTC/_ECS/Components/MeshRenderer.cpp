#include "MeshRenderer.h"

#include "_graphics/_vulkan/GraphicsPipeline.h"
#include "_ECS/GameObject.h"
#include "_ECS/Components/Transform.h"
#include "_graphics/_vulkan/Buffer.h"
#include "_resources/Material.h"

MeshRenderer::~MeshRenderer() {
  delete mesh;
  delete material;
}
