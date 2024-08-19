#pragma once

#include "entt.hpp"
#include "Utils.h"
#include "Resources.h"

#include <vector>
#include <optional>

class GameObject;

namespace ECS {

	struct BaseComponent {
	public:
		static inline GameObject* gameObjectBuffer{ nullptr };
		GameObject* gameObject{ nullptr };
	protected:
		BaseComponent() {
			gameObject = gameObjectBuffer;
			gameObjectBuffer = nullptr;
		};
	};

	struct ObjectData : public BaseComponent {
		std::vector<const char*> tags;
		bool isActive{ true };

		uint32_t childCount{};
		// First child
		entt::entity first{ entt::null };
		// Previous sibling
		entt::entity prev{ entt::null };
		// Next sibling
		entt::entity next{ entt::null };
		entt::entity parent{ entt::null };

		entt::entity rootParent{ entt::null };
	};
}

struct Transform : public ECS::BaseComponent {
	vec3 position{ 0,0,0 };
	quat rotation{ 0,0,0,1 };
	vec3 scale{ 1,1,1 };

	Transform() = default;
	Transform(vec3 position, quat rotation, vec3 scale)
		: position{ position }, rotation{ rotation }, scale{ scale } {}
	Transform(const Transform& t) : Transform(t.position, t.rotation, t.scale) {}

	mat4 getModelMatrix();
	void rotate(vec3 angles);
};

//struct SpriteRenderer : public ECS::BaseComponent {
//	Image sprite;
//
//	SpriteRenderer() = default;
//	SpriteRenderer(Image sprite) : sprite{ sprite } {};
//	SpriteRenderer(const SpriteRenderer& sr) : SpriteRenderer(sr.sprite) {};
//};

struct MeshRenderer : public ECS::BaseComponent {
	Mesh mesh;
	//Material material;

	MeshRenderer() = default;
	MeshRenderer(Mesh mesh/*, Material material*/) : mesh{ mesh }/*, material{ material }*/ {};
	MeshRenderer(const MeshRenderer& mr) : MeshRenderer(mr.mesh/*, mr.material*/) {};
};

enum CameraProjections {
	PERSPECTIVE, ORTHOGRAPHIC
};

struct Camera : public ECS::BaseComponent {
private:
	Transform* transform;

public:
	mat4 view;
	mat4 projection;
	CameraProjections projectionType{ PERSPECTIVE };

	void updateView();
	void updateProjectionPerspective(float fovY, float aspect, float zNear, float zFar);
	void updateProjectionOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);

	Camera();
	Camera(CameraProjections projectionType) : Camera() { this->projectionType = projectionType; }
	Camera(const Camera& c) : Camera(c.projectionType) {}
};

class GameObject {
private:
	static inline entt::registry _registry;

	entt::entity _id;

public:
	GameObject();
	~GameObject();

	template<typename Type>
	Type& getComponent() { return _registry.get<Type>(_id); }

	template<typename Type>
	bool hasComponent() { return _registry.all_of<Type>(_id); }

	template<typename Type, typename... Args>
	Type& addComponent(Args &&... args) {
		static_assert(std::is_base_of<ECS::BaseComponent, Type>::value, "Components must be derived from BaseComponent");

		if (hasComponent<Type>()) return getComponent<Type>();

		ECS::BaseComponent::gameObjectBuffer = this;

		return _registry.emplace<Type>(_id, args...);
	}

	template<typename Type>
	void removeComponent() {
		_registry.remove<Type>(_id);
	}

	/*void getComponentAtIndex();
	void getComponentIndex();
	int getComponentCount();*/

	template<typename Type>
	Type getComponentInChildren() {
		ECS::ObjectData& data = getComponent<ECS::ObjectData>();

		entt::entity currentChild = data.first;
		for (std::uint32_t i = 0; i < data.childCount; ++i) {
			if (_registry.all_of<Type>(currentChild)) {
				return _registry.get<Type>(currentChild);
			}
			currentChild = _registry.get<ECS::ObjectData>(currentChild).next;
		}

		throw Exception("No children have the necessary component.");
	}

	template<typename Type>
	std::vector<Type*> getComponentsInChildren() {
		ECS::ObjectData& data = getComponent<ECS::ObjectData>();

		std::vector<Type> components{};

		entt::entity currentChild = data.first;
		for (std::uint32_t i = 0; i < data.childCount; ++i) {
			if (_registry.all_of<Type>(currentChild)) {
				components.push_back(_registry.get<Type>(currentChild));
			}
			currentChild = _registry.get<ECS::ObjectData>(currentChild).next;
		}

		int size = (int)components.size();
		std::vector<Type*> componentsResult(size);
		for (int i = 0; i < size; i++) {
			componentsResult[i] = &components[i];
		}

		return componentsResult;
	}

	template<typename Type>
	Type getComponentInParent() {
		ECS::ObjectData& data = getComponent<ECS::ObjectData>();

		if (data.parent == entt::null) {
			throw Exception("This GameObject doesn't have any parent.");
		}

		if (_registry.all_of<Type>(data.parent)) {
			return _registry.get<Type>(data.parent);
		}

		throw Exception("This GameObject's parent doesn't have the necessary component.");
	}

	bool hasTag(const char* tag);
	void addTag(const char* tag);
	void removeTag(const char* tag);

	bool isActive();
	void setActive(bool isActive);

	entt::entity getId() const;

	bool hasParent();
	void setParent(GameObject& parent);
	void removeParent();

	int getChildrenCount();
	bool isChild(GameObject& object);
	void addChild(GameObject& child);
	void removeChild(GameObject& child);

	/*static void createPrimitive();
	static void find();
	static void findWithTag();
	static void findGameObjectsWithTag();*/
};

class World : public GameObject {
public:
	std::vector<Vertex> triangles;
	std::vector<uint32_t> indices;
	Camera* camera;

	void addChild(GameObject& child);
};