#pragma once

#include "entt/entt.hpp"
//#include "_utils/Utils.h"
#include "Components/BaseComponent.h"

#include <vector>

class GameObject;

namespace ECS {
  struct ObjectData : public BaseComponent {
    std::vector<const char*> tags;
    bool isActive{ true };

    uint32_t childCount{0};
    // First child
    GameObject* first{ nullptr };
    // Previous sibling
    GameObject* prev{ nullptr };
    // Next sibling
    GameObject* next{ nullptr };
    GameObject* parent{ nullptr };

    GameObject* rootParent{ nullptr };
  };
}

class GameObject {
private:
  static inline entt::registry _registry;

  entt::entity _id;

public:
  GameObject();
  ~GameObject();

  template<typename Type>
  Type* getComponent() const {
    if(_registry.valid(_id) && _registry.all_of<Type>(_id)) {
      return &_registry.get<Type>(_id);
    }
    return nullptr;
  }

  template<typename Type>
  bool hasComponent() const {
    return _registry.valid(_id) && _registry.all_of<Type>(_id);
  }

  template<typename Type, typename... Args>
  Type* addComponent(Args &&... args) {
    static_assert(std::is_base_of<ECS::BaseComponent, Type>::value, "Components must be derived from BaseComponent");

    if (_registry.all_of<Type>(_id)) return &_registry.get<Type>(_id);

    ECS::BaseComponent::gameObjectBuffer = this;

    return &_registry.emplace<Type>(_id, args...);
  }

  template<typename Type>
  void removeComponent() const {
    _registry.remove<Type>(_id);
  }
  
  template<typename Type>
  Type* getComponentInChildren() {
    ECS::ObjectData& data = _registry.get<ECS::ObjectData>(_id);

    entt::entity currentChild = data.first->_id;
    for (std::uint32_t i = 0; i < data.childCount; ++i) {
      if (_registry.all_of<Type>(currentChild)) {
        return &_registry.get<Type>(currentChild);
      }
      currentChild = _registry.get<ECS::ObjectData>(currentChild).next->_id;
    }

    return nullptr;
  }

  template<typename Type>
  std::vector<Type*> getComponentsInChildren() {
    std::vector<Type*> components{};

    entt::runtime_view view{};
    view.iterate(_registry.storage<Type>());

    for (auto entity : view) {
      components.push_back(&_registry.get<Type>(entity));
    }

    return components;
  }

  template<typename Type>
  Type* getComponentInParent() {
    ECS::ObjectData& data = _registry.get<ECS::ObjectData>(_id);

    if (data.parent != entt::null && _registry.all_of<Type>(data.parent)) {
      return &_registry.get<Type>(data.parent);
    }

    return nullptr;
  }

  bool hasTag(const char* tag) const;
  void addTag(const char* tag) const;
  void removeTag(const char* tag) const;

  bool isActive() const;
  void setActive(bool isActive) const;

  entt::entity getId() const;

  bool hasParent() const;
  void setParent(GameObject* parent) const;
  void removeParent() const;

  int getChildrenCount() const;
  bool isChild(const GameObject* object) const;
  void addChild(GameObject* child) const;
  void removeChild(const GameObject* child) const;
  GameObject* getChild(uint32_t index) const;

  /*void getComponentAtIndex();
  void getComponentIndex();
  int getComponentCount();
  static GameObject createPrimitive();
  static GameObject find();
  static GameObject findWithTag();
  static std::vector<GameObject> findGameObjectsWithTag();*/

  bool operator==(const GameObject* obj) const { return _id == obj->_id; }
  bool operator!=(const GameObject* obj) const { return !(*this == obj); }
};