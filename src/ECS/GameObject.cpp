#include "GameObject.h"

#include "Components/MeshRenderer.h"
#include "resources/Material.h"
#include "Components/Transform.h"
#include "Components/Camera.h"

using namespace ECS;

GameObject::GameObject() {
  _id = _registry.create();
  addComponent<ObjectData>();
  addComponent<Transform>();
}

GameObject::~GameObject() {
  const ObjectData& data = _registry.get<ObjectData>(_id);

  // recur through children
  GameObject* currentChild = data.first;
  GameObject* nextChild{ nullptr };
  for (uint32_t i = 0; i < data.childCount; i++) {
    nextChild = _registry.get<ObjectData>(currentChild->_id).next;

    currentChild->~GameObject();

    currentChild = nextChild;
  }

  // free id
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
  return data.parent != data.rootParent && data.parent != nullptr;
}

void GameObject::setParent(GameObject* parent) const {
  _registry.get<ObjectData>(_id).parent = parent;
}

void GameObject::removeParent() const {
  ObjectData data = _registry.get<ObjectData>(_id);
  data.parent = data.rootParent;
}

int GameObject::getChildrenCount() const {
  return (int)_registry.get<ObjectData>(_id).childCount;
}

bool GameObject::isChild(const GameObject* object) const {
  const ObjectData& data = _registry.get<ObjectData>(_id);

  if (data.childCount == 0) return false;

  const ObjectData& objData = _registry.get<ObjectData>(object->_id);

  if (objData.parent)
    return objData.parent->operator==(this);

  return false;
}

void GameObject::addChild(GameObject* child) const {
  if (isChild(child)) return;

  ObjectData& data = _registry.get<ObjectData>(_id);

  ObjectData& childData = _registry.get<ObjectData>(child->_id);
  childData.parent = (GameObject*)this;
  childData.rootParent = data.rootParent == nullptr ? (GameObject*)this : data.rootParent;

  if (data.childCount == 0) {
    data.first = child;
  }
  else {
    GameObject* currentChild = data.first;
    for (std::uint32_t i = 0; i < data.childCount - 1; i++) {
      currentChild = _registry.get<ObjectData>(currentChild->_id).next;
    }

    _registry.get<ObjectData>(currentChild->_id).next = (GameObject*)child;
    _registry.get<ObjectData>(child->_id).prev = currentChild;
  }

  data.childCount++;
}

void GameObject::removeChild(const GameObject* child) const {
  if (isChild(child)) return;

  ObjectData& data = _registry.get<ObjectData>(_id);

  if (child->operator==(data.first)) {
    data.first = _registry.get<ObjectData>(child->_id).next;
    return;
  }

  GameObject* currentChild = data.first;
  GameObject* nextChild = _registry.get<ObjectData>(currentChild->_id).next;
  for (uint32_t i = 0; i < data.childCount; ++i) {
    if (nextChild->operator==(child)) {
      _registry.get<ObjectData>(currentChild->_id).next = _registry.get<ObjectData>(nextChild->_id).next;
      return;
    }
    currentChild = _registry.get<ObjectData>(currentChild->_id).next;
    nextChild = _registry.get<ObjectData>(currentChild->_id).next;
  }

  data.childCount--;
}

GameObject* GameObject::getChild(uint32_t index) const {
  ObjectData& data = _registry.get<ObjectData>(_id);

  if (index > data.childCount - 1) return nullptr;

  data = _registry.get<ObjectData>(data.first->_id);

  for (uint32_t i = 0; i < index - 1; i++) {
    data = _registry.get<ObjectData>(data.next->_id);
  }

  return data.next;
}
