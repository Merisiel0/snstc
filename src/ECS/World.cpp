#include "World.h"

#include "entt/entt.hpp"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Light.h"
#include "graphics/vulkan/Buffer.h"

World::World(std::vector<GameObject*> gameObjects) {
  for (const auto& child : gameObjects) {
    addChild(child);
  }

  _camera = getComponentInChildren<Camera>();
  
  camBuffer = new Buffer(sizeof(CameraBuffer), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
  lightsBuffer = new Buffer(sizeof(LightBuffer), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}

void World::addGameObject(GameObject* obj) {
  if (_camera == nullptr && obj->hasComponent<Camera>()) {
    _camera = obj->getComponent<Camera>();
  }

  GameObject::addChild(obj);
}

void World::updateCamera(CommandBuffer* commandBuffer) {
  _camera->updateView();

  CameraBuffer camBuf{};
  camBuf.projView = _camera->projection * _camera->view;

  camBuffer->update<CameraBuffer>(commandBuffer, { camBuf });

  LightBuffer lightBuf{};
  AmbiantLight* ambiant = getComponentInChildren<AmbiantLight>();
  lightBuf.ambiantColor = ambiant->color;

  Light* light = getComponentInChildren<Light>();
  lightBuf.position = light->gameObject->getComponent<Transform>()->truePosition();
  lightBuf.color = light->color;
  lightBuf.viewPosition = _camera->gameObject->getComponent<Transform>()->truePosition();

  lightsBuffer->update<LightBuffer>(commandBuffer, { lightBuf });
}
