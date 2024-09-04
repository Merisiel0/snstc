#pragma once

#include "entt.hpp"
#include "_utils/Utils.h"
#include "Components/BaseComponent.h"

#include <vector>

namespace ECS {
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

class GameObject {
private:
  static inline entt::registry _registry;

  entt::entity _id;

public:
  GameObject();
  ~GameObject();

  template<typename Type>
  Type* getComponent() const { return &_registry.get<Type>(_id); }

  template<typename Type>
  bool hasComponent() const { return _registry.all_of<Type>(_id); }

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

    entt::entity currentChild = data.first;
    for (std::uint32_t i = 0; i < data.childCount; ++i) {
      if (_registry.all_of<Type>(currentChild)) {
        return &_registry.get<Type>(currentChild);
      }
      currentChild = _registry.get<ECS::ObjectData>(currentChild).next;
    }

    throw Exception("No children have the necessary component.");
  }

  template<typename Type>
  std::vector<Type*> getComponentsInChildren() {
    /*ECS::ObjectData& data = _registry.get<ECS::ObjectData>(_id);

    std::vector<Type*> components{};

    entt::entity currentChild = data.first;
    for (std::uint32_t i = 0; i < data.childCount; ++i) {
      if (_registry.all_of<Type>(currentChild)) {
        components.push_back(&_registry.get<Type>(currentChild));
      }

      std::vector<Type*> childComponents = getComponentsInChildren(currentChild);
      components.insert(components.end(), childComponents.begin(), childComponents.end());

      currentChild = _registry.get<ECS::ObjectData>(currentChild).next;
    }*/

    /*int size = (int)components.size();
    std::vector<Type*> componentsResult(size);
    for (int i = 0; i < size; i++) {
      componentsResult[i] = &components[i];
    }*/

    std::vector<Type*> components{};

    entt::runtime_view view{};
    view.iterate(_registry.storage<Type>());

    for (auto entity : view) {
      components.push_back(&view.get<Type>(entity));
    }

    return components;
  }

  template<typename Type>
  Type* getComponentInParent() {
    ECS::ObjectData& data = _registry.get<ECS::ObjectData>(_id);

    if (data.parent == entt::null) {
      throw Exception("This GameObject doesn't have any parent.");
    }

    if (_registry.all_of<Type>(data.parent)) {
      return &_registry.get<Type>(data.parent);
    }

    throw Exception("This GameObject's parent doesn't have the necessary component.");
  }

  bool hasTag(const char* tag) const;
  void addTag(const char* tag) const;
  void removeTag(const char* tag) const;

  bool isActive() const;
  void setActive(bool isActive) const;

  entt::entity getId() const;

  bool hasParent() const;
  void setParent(const GameObject& parent) const;
  void removeParent() const;

  int getChildrenCount() const;
  bool isChild(const GameObject& object) const;
  void addChild(const GameObject& child) const;
  void removeChild(const GameObject& child) const;

  /*void getComponentAtIndex();
  void getComponentIndex();
  int getComponentCount();
  static GameObject createPrimitive();
  static GameObject find();
  static GameObject findWithTag();
  static std::vector<GameObject> findGameObjectsWithTag();*/
};