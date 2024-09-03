#include "World.h"

#include "entt.hpp"
#include "Components/Camera.h"
#include "../_graphics/TLAS.h"

void World::updatePhysics() {

}

void World::updateGraphics() {

}

World::World(std::vector<GameObject> gameObjects) {
	for (const auto& child : gameObjects) {
		addChild(child);
	}

	_camera = getComponentInChildren<Camera>();
}

void World::addGameObject(GameObject& obj) {
	if (_camera == nullptr && obj.hasComponent<Camera>()) {
		_camera = obj.getComponent<Camera>();
	}

	GameObject::addChild(obj);
}