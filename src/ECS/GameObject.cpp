#include "GameObject.h"

#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "World.h"
#include "resources/Material.h"
#include "resources/Mesh.h"

using namespace ECS;

void GameObject::setWorldCamera(Camera& cam) {
  const ObjectData& data = _registry->get<ObjectData>(_id);
  data.world->_camera = &cam;
}

GameObject::GameObject(entt::registry& registry) { _registry = &registry; }

GameObject::GameObject(World* world) {
  _registry = &world->_registry;
  _id = _registry->create();

  ObjectData& data = addComponent<ObjectData>();
  data.world = world;

  addComponent<Transform>();

  world->addChild(*this);
}

GameObject::~GameObject() {
  if(_id == entt::null) return;

  const ObjectData& data = _registry->get<ObjectData>(_id);

  if(data.tags.size() > 0) { std::cout << "Destroyed " << data.tags[0] << std::endl; }

  // recursively destroy all children
  GameObject* currentChild = data.first;
  GameObject* nextChild {nullptr};
  for(uint32_t i = 0; i < data.childCount; i++) {
    nextChild = _registry->get<ObjectData>(currentChild->_id).next;

    currentChild->~GameObject();

    currentChild = nextChild;
  }

  // remove from parent
  if(data.parent) { data.parent->removeChild(*this); }

  _registry->destroy(_id);
  _registry = nullptr;
}

bool GameObject::hasTag(std::string tag) const {
  const ObjectData& data = _registry->get<ObjectData>(_id);

  if(data.tags.size() == 0) return false;

  auto it = std::find(data.tags.begin(), data.tags.end(), tag);
  return it != data.tags.end();
}

void GameObject::addTag(std::string tag) const {
  if(hasTag(tag)) return;

  _registry->get<ObjectData>(_id).tags.push_back(tag);
}

std::vector<std::string> GameObject::getTags() const {
  return _registry->get<ObjectData>(_id).tags;
}

void GameObject::removeTag(std::string tag) const {
  if(!hasTag(tag)) return;

  ObjectData& data = _registry->get<ObjectData>(_id);

  auto it = std::find(data.tags.begin(), data.tags.end(), tag);
  if(it != data.tags.end()) { data.tags.erase(it); }
}

bool GameObject::isActive() const { return _registry->get<ObjectData>(_id).isActive; }

void GameObject::setActive(bool isActive) const {
  _registry->get<ObjectData>(_id).isActive = isActive;
}

entt::entity GameObject::getId() const { return _id; }

bool GameObject::hasParent() const {
  const ObjectData& data = _registry->get<ObjectData>(_id);
  return data.parent != data.world && data.parent != nullptr;
}

void GameObject::setParent(GameObject& parent) const {
  ObjectData& data = _registry->get<ObjectData>(_id);
  ObjectData& newParentData = _registry->get<ObjectData>(parent._id);

  data.parent->removeChild(*this);
  data.parent = &parent;
  data.world = newParentData.world;
}

void GameObject::removeParent() const {
  ObjectData& data = _registry->get<ObjectData>(_id);
  data.parent = data.world;
}

int GameObject::getChildrenCount() const {
  const ObjectData& data = _registry->get<ObjectData>(_id);
  return (int) data.childCount;
}

bool GameObject::isChild(const GameObject& object) const {
  const ObjectData& data = _registry->get<ObjectData>(_id);

  if(data.childCount == 0) return false;

  const ObjectData& objData = _registry->get<ObjectData>(object._id);

  if(objData.parent) return objData.parent->operator==(this);

  return false;
}

void GameObject::addChild(GameObject& child) const {
  if(isChild(child) || this->operator==(child)) return;

  ObjectData& data = _registry->get<ObjectData>(_id);

  ObjectData& childData = _registry->get<ObjectData>(child._id);
  childData.parent = (GameObject*) this;

  if(data.childCount == 0) {
    data.first = &child;
  } else {
    GameObject* currentChild = data.first;
    for(std::uint32_t i = 0; i < data.childCount - 1; i++) {
      currentChild = _registry->get<ObjectData>(currentChild->_id).next;
    }

    _registry->get<ObjectData>(currentChild->_id).next = &child;
    childData.prev = currentChild;
  }

  data.childCount++;
}

void GameObject::removeChild(const GameObject& child) const {
  if(!isChild(child)) return;

  ObjectData& data = _registry->get<ObjectData>(_id);

  if(child.operator==(data.first)) {
    data.first = _registry->get<ObjectData>(child._id).next;
    data.childCount--;
    return;
  }

  GameObject* currentChild = data.first;
  GameObject* nextChild = _registry->get<ObjectData>(currentChild->_id).next;
  for(uint32_t i = 0; i < data.childCount; ++i) {
    if(nextChild->operator==(child)) {
      _registry->get<ObjectData>(currentChild->_id).next =
        _registry->get<ObjectData>(nextChild->_id).next;
      data.childCount--;
      return;
    }
    currentChild = _registry->get<ObjectData>(currentChild->_id).next;
    nextChild = _registry->get<ObjectData>(currentChild->_id).next;
  }
}

GameObject* GameObject::getChild(uint32_t index) const {
  const ObjectData& data = _registry->get<ObjectData>(_id);

  if(index > data.childCount - 1) return nullptr;

  ObjectData* current = &_registry->get<ObjectData>(data.first->_id);

  for(uint32_t i = 0; i < index - 1; i++) {
    current = &_registry->get<ObjectData>(data.next->_id);
  }

  return current->next;
}

GameObject* GameObject::createPrimitive(
  World* world, GameObjectPrimitives primitive, std::shared_ptr<Material> material) {
  GameObject* obj = new GameObject(world);

  std::shared_ptr<Mesh> mesh;
  switch(primitive) {
    case PLANE:
      mesh = Mesh::generatePlane({1, 1}, {2, 2});
      obj->addComponent<MeshRenderer>(mesh, material);
      break;

      // case CUBE:
      //   mesh = ResourceManager::generatePlane({1, 1}, {2, 2});
      // // TODO: change to InstanceRenderer
      //   MeshRenderer& mr = obj.addComponent<MeshRenderer>(mesh, material);

      //   mr.setMaxInstanceCount(6);

      //   // top plane
      //   InstanceProperties props;
      //   mat4& mat = props.transform;
      //   mat = glm::identity<mat4>();
      //   mat = glm::translate(mat, vec3 {0, 0.5f, 0});
      //   mr.addInstance(props);

      //   // bottom plane
      //   mat = glm::identity<mat4>();
      //   mat = glm::rotate(mat, radians(180.f), vec3{1,0,0});
      //   mat = glm::translate(mat, {0, 0.5f, 0});
      //   mr.addInstance(props);

      //   // right plane
      //   mat = glm::identity<mat4>();
      //   mat = glm::rotate(mat, radians(90.f), vec3{0,0,1});
      //   mat = glm::translate(mat, {0, 0.5f, 0});
      //   mr.addInstance(props);

      //   // left plane
      //   mat = glm::identity<mat4>();
      //   mat = glm::rotate(mat, radians(-90.f), vec3{0,0,1});
      //   mat = glm::translate(mat, {0, 0.5f, 0});
      //   mr.addInstance(props);

      //   // front plane
      //   mat = glm::identity<mat4>();
      //   mat = glm::rotate(mat, radians(90.f), vec3{1,0,0});
      //   mat = glm::translate(mat, {0, 0.5f, 0});
      //   mr.addInstance(props);

      //   // back plane
      //   mat = glm::identity<mat4>();
      //   mat = glm::rotate(mat, radians(-90.f), vec3{1,0,0});
      //   mat = glm::translate(mat, {0, 0.5f, 0});
      //   mr.addInstance(props);
      //   break;
  }

  return obj;
}
