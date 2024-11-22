#pragma once

#include "GameObject.h"
#include <vector>

struct Camera;
class Buffer;
class CommandBuffer;

class World : public GameObject {
private:
	using GameObject::addChild;

	Camera* _camera;

public:
	Buffer* camBuffer;
	Buffer* lightsBuffer;

	World(std::vector<GameObject*> gameObjects);

	void addGameObject(GameObject* obj);

	void updateCamera(CommandBuffer* commandBuffer);

	Camera* camera() const { return _camera; }
};