#include "GameObject.h"

using namespace ECS;

GameObject::GameObject() {
	_id = _registry.create();
	addComponent<ObjectData>();
}

GameObject::~GameObject() {
	const ObjectData& data = _registry.get<ObjectData>(_id);

	if (data.childCount > 0) {
		entt::entity currentChild = data.first;
		for (uint32_t i = 0; i < data.childCount; i++) {
			_registry.get<ObjectData>(currentChild).parent = data.parent;
			currentChild = _registry.get<ObjectData>(currentChild).next;
		}
	}

	if (data.prev != entt::null) {
		_registry.get<ObjectData>(data.prev).next = data.next;
	}
	if (data.next != entt::null) {
		_registry.get<ObjectData>(data.next).prev = data.prev;
	}

	if (data.parent != entt::null) {
		ObjectData& parentData = _registry.get<ObjectData>(data.parent);
		if (parentData.childCount == 1) {
			parentData.first = entt::null;
		}
		else {
			entt::entity currentSibling = parentData.first;
			for (uint32_t i = 0; i < parentData.childCount; i++) {
				ObjectData& currentSiblingData = _registry.get<ObjectData>(currentSibling);
				if (currentSiblingData.next == _id) {
					currentSiblingData.next = _registry.get<ObjectData>(currentSiblingData.next).next;
					break;
				}
				currentSibling = _registry.get<ObjectData>(currentSibling).next;
			}
		}
		parentData.childCount--;
	}

	_registry.destroy(_id);
}

bool GameObject::hasTag(const char* tag) const {
	const ObjectData& data = _registry.get<ObjectData>(_id);

	if (data.tags.size() == 0) return false;

	auto it = std::find(data.tags.begin(), data.tags.end(), tag);
	return it != data.tags.end();
}

void GameObject::addTag(const char* tag) const {
	if (!hasTag(tag)) {
		_registry.get<ObjectData>(_id).tags.push_back(tag);
	}
}

void GameObject::removeTag(const char* tag) const {
	if (!hasTag(tag)) return;

	ObjectData& data = _registry.get<ObjectData>(_id);

	auto it = std::find(data.tags.begin(), data.tags.end(), tag);
	if (it != data.tags.end()) {
		data.tags.erase(it);
	}
}

bool GameObject::isActive() const {
	return _registry.get<ObjectData>(_id).isActive;
}

void GameObject::setActive(bool isActive) const {
	_registry.get<ObjectData>(_id).isActive = isActive;
}

entt::entity GameObject::getId() const {
	return _id;
}

bool GameObject::hasParent() const {
	const ObjectData& data = _registry.get<ObjectData>(_id);
	return data.parent != data.rootParent && data.parent != entt::null;
}

void GameObject::setParent(const GameObject& parent) const {
	_registry.get<ObjectData>(_id).parent = parent._id;
}

void GameObject::removeParent() const {
	ObjectData& data = _registry.get<ObjectData>(_id);
	data.parent = data.rootParent;
}

int GameObject::getChildrenCount() const {
	return (int)_registry.get<ObjectData>(_id).childCount;
}

bool GameObject::isChild(const GameObject& object) const {
	const ObjectData& data = _registry.get<ObjectData>(_id);

	if (data.childCount == 0) return false;

	return _registry.get<ObjectData>(object._id).parent == _id;
}

void GameObject::addChild(const GameObject& child) const {
	if (!isChild(child)) {
		ObjectData& data = _registry.get<ObjectData>(_id);

		ObjectData& childData = _registry.get<ObjectData>(child._id);
		childData.parent = _id;
		childData.rootParent = data.rootParent == entt::null ? _id : data.rootParent;

		if (data.childCount == 0) {
			data.first = child._id;
		}
		else {
			entt::entity currentChild = data.first;
			for (std::uint32_t i = 0; i < data.childCount - 1; i++) {
				currentChild = _registry.get<ObjectData>(currentChild).next;
			}

			_registry.get<ObjectData>(currentChild).next = child._id;
			_registry.get<ObjectData>(child._id).prev = currentChild;
		}

		data.childCount++;
	}
}

void GameObject::removeChild(const GameObject& child) const {
	if (!isChild(child)) {
		ObjectData& data = _registry.get<ObjectData>(_id);

		if (child._id == data.first) {
			data.first = _registry.get<ObjectData>(child._id).next;
			return;
		}

		entt::entity currentChild = data.first;
		entt::entity nextChild = _registry.get<ObjectData>(currentChild).next;
		for (uint32_t i = 0; i < data.childCount; ++i) {
			if (nextChild == child._id) {
				_registry.get<ObjectData>(currentChild).next = _registry.get<ObjectData>(nextChild).next;
				return;
			}
			currentChild = _registry.get<ObjectData>(currentChild).next;
			nextChild = _registry.get<ObjectData>(currentChild).next;
		}

		data.childCount--;
	}
}