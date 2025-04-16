#pragma once

#include "GameObject.h"

#include <vector>

struct Camera;
class Buffer;
class CommandBuffer;

class World : public GameObject {
private:
  using GameObject::setParent;
  friend class GameObject;

  entt::registry _registry;

  Camera* _camera;

public:
  Buffer* camBuffer;
  Buffer* lightsBuffer;

  World();

  ~World();

  void updateCamera(std::shared_ptr<CommandBuffer> commandBuffer);

  Camera* camera() const { return _camera; }
};