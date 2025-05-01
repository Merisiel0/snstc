#include "World.h"

#include "Components/Camera.h"
#include "Components/Light.h"
#include "Components/Transform.h"
#include "entt/entt.hpp"
#include "graphics/vulkan/Buffer.h"

using namespace ECS;

World::World() : GameObject(_registry) {
  _id = _registry.create();

  addComponent<ECS::ObjectData>();
  addComponent<Transform>();

  camBuffer = new Buffer(sizeof(CameraBuffer),
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  lightsBuffer = new Buffer(sizeof(LightBuffer),
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
}

World::~World() {
  const ObjectData& data = _registry.get<ObjectData>(_id);

  // recursively destroy all children
  GameObject* currentChild = data.first;
  GameObject* nextChild {nullptr};
  for(uint32_t i = 0; i < data.childCount; i++) {
    nextChild = _registry.get<ObjectData>(currentChild->_id).next;

    currentChild->~GameObject();

    currentChild = nextChild;
  }

  _registry.destroy(_id);
  _id = entt::null;

  delete camBuffer;
  delete lightsBuffer;
}

void World::updateCamera(std::shared_ptr<CommandBuffer> commandBuffer) {
  if(!_camera) return;

  _camera->updateView();

  CameraBuffer camBuf {};
  camBuf.projection = _camera->projection;
  camBuf.view = _camera->view;

  camBuffer->update<CameraBuffer>(commandBuffer, {camBuf});

  LightBuffer lightBuf {};
  AmbiantLight* ambiant = getComponentInChildren<AmbiantLight>();
  lightBuf.ambiantColor = ambiant ? ambiant->color : Color {0, 0, 0, 0};

  Light* light = getComponentInChildren<Light>();
  if(light) {
    lightBuf.position = light->gameObject->getComponent<Transform>()->getPosition(WORLD);
    lightBuf.color = light->color;
  } else {
    lightBuf.position = vec3 {0, 0, 0};
    lightBuf.color = Color {0, 0, 0, 0};
  }

  lightBuf.viewPosition = _camera->gameObject->getComponent<Transform>()->getPosition(WORLD);

  lightsBuffer->update<LightBuffer>(commandBuffer, {lightBuf});
}
