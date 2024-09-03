#pragma once

#include "GameObject.h"
#include <vector>

struct Camera;
class TLAS;
struct VkAccelerationStructureInstanceKHR;

class World : public GameObject {
private:
	using GameObject::addChild;

	Camera* _camera;
	TLAS* _accelerationStructure;

	void updatePhysics();
	void updateGraphics();

public:
	World(std::vector<GameObject> gameObjects);

	void addGameObject(GameObject& obj);
};