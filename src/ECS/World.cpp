#include "World.h"

#include "Components/Camera.h"
#include "Components/Light.h"
#include "Components/Transform.h"
#include "entt/entt.hpp"
#include "graphics/vulkan/Buffer.h"

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
  delete camBuffer;
  delete lightsBuffer;
}

void World::updateCamera(std::shared_ptr<CommandBuffer> commandBuffer) {
  if(!_camera) return;

  _camera->updateView();

  CameraBuffer camBuf {};
  camBuf.projView = _camera->projection * _camera->view;

  camBuffer->update<CameraBuffer>(commandBuffer, {camBuf});

  LightBuffer lightBuf {};
  AmbiantLight* ambiant = getComponentInChildren<AmbiantLight>();
  lightBuf.ambiantColor = ambiant ? ambiant->color : Color {0, 0, 0, 0};

  Light* light = getComponentInChildren<Light>();
  if(light) {
    lightBuf.position = light->gameObject->getComponent<Transform>()->truePosition();
    lightBuf.color = light->color;
  } else {
    lightBuf.position = vec3 {0, 0, 0};
    lightBuf.color = Color {0, 0, 0, 0};
  }

  lightBuf.viewPosition = _camera->gameObject->getComponent<Transform>()->truePosition();

  lightsBuffer->update<LightBuffer>(commandBuffer, {lightBuf});
}
