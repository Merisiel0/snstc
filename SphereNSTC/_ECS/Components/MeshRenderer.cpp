#include "MeshRenderer.h"

#include "_graphics/GraphicsPipeline.h"
#include "_ECS/GameObject.h"
#include "_ECS/Components/Transform.h"
#include "_graphics/Buffer.h"

PushConstants MeshRenderer::getPushConstants() {
  PushConstants constants{};
  constants.transform = gameObject->getComponent<Transform>()->getModelMatrix();
  constants.vertexBuffer = mesh->vertices->address;

  return constants;
}
