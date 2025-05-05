#pragma once

#include "Components/BaseComponent.h"
#include "entt/entt.hpp"

#include <string>
#include <vector>

class GameObject;
class World;
class Camera;
class Material;

namespace ECS {
  struct ObjectData : public BaseComponent {
    std::vector<std::string> tags {};
    bool isActive {true};

    uint32_t childCount {0};
    // First child
    GameObject* first {nullptr};
    // Previous sibling
    GameObject* prev {nullptr};
    // Next sibling
    GameObject* next {nullptr};
    GameObject* parent {nullptr};

    World* world {nullptr};
  };
} // namespace ECS

enum GameObjectPrimitives { PLANE, CUBE, SPHERE };

/// @brief A GameObject entity inside a digital World.
class GameObject {
private:
  entt::registry* _registry;

  entt::entity _id;

  /// @brief GameObject constructor reserved for World to instantiate.
  /// @param registry a world's registry.
  GameObject(entt::registry& registry);
  friend class World;

  /// @brief Sets this GameObject's world's camera.
  /// @param cam a camera component.
  void setWorldCamera(Camera& cam);

public:
  GameObject(World& world);
  ~GameObject();

  /// @brief GameObjects should be unique, copying them is not allowed.
  ///
  /// Thist constructor is deleted.
  GameObject(const GameObject&) = delete;

  /// @brief GameObjects should be unique, copying them is not allowed.
  ///
  /// Thist operator is deleted.
  GameObject& operator=(const GameObject&) = delete;

  GameObject(GameObject&&) = default;
  GameObject& operator=(GameObject&&) = default;

  /// @brief Gets a component on this GameObject.
  /// @tparam Type type of the component to get.
  /// @return the component if it exists, nullptr otherwise.
  template<typename Type>
  Type* getComponent() const {
    if(_registry->valid(_id) && _registry->all_of<Type>(_id)) { return &_registry->get<Type>(_id); }
    return nullptr;
  }

  /// @brief Checks if this GameObject has a component.
  /// @tparam Type type of the component to check.
  /// @return true if this GameObject has the specified component, false otherwise.
  template<typename Type>
  bool hasComponent() const {
    return _registry->valid(_id) && _registry->all_of<Type>(_id);
  }

  /// @brief Adds a component on this GameObject.
  /// @tparam Type type of the component to add.
  /// @tparam ...Args generic type for the component's constructor's arguments.
  /// @param ...args arguments for the component constructor.
  /// @return a reference to the newly created component.
  template<typename Type, typename... Args>
  Type& addComponent(Args&&... args) {
    static_assert(std::is_base_of<ECS::BaseComponent, Type>::value,
      "Components must be derived from BaseComponent");

    if(_registry->all_of<Type>(_id)) return _registry->get<Type>(_id);

    ECS::BaseComponent::gameObjectBuffer = this;

    Type& component = _registry->emplace<Type>(_id, args...);

    if constexpr(std::is_same_v<Type, Camera>) { setWorldCamera(component); }

    return component;
  }

  /// @brief Removes a component on this GameObject if it exists.
  /// @tparam Type type of the component to remove.
  /// @return true if a component was removed, false otherwise.
  template<typename Type>
  bool removeComponent() const {
    if(_registry->valid(_id) && _registry->all_of<Type>(_id)) {
      _registry->remove<Type>(_id);
      return true;
    }
    return false;
  }

  /// @brief Gets the first occurence of a component on this GameObject's children.
  /// @tparam Type type of the component to get.
  /// @return the component if it exists, nullptr otherwise.
  template<typename Type>
  Type* getComponentInChildren() {
    if(!_registry->valid(_id)) return nullptr;

    ECS::ObjectData& data = _registry->get<ECS::ObjectData>(_id);

    if(data.childCount == 0) return nullptr;

    GameObject* currentChild = data.first;

    for(std::uint32_t i = 0; i < data.childCount; ++i) {
      if(_registry->all_of<Type>(currentChild->_id)) {
        return &_registry->get<Type>(currentChild->_id);
      }
      currentChild = _registry->get<ECS::ObjectData>(currentChild->_id).next;
    }

    return nullptr;
  }

  /// @brief Gets all the occurences of a component on this GameObject's children.
  /// @tparam Type type of a component to get.
  /// @return an array of references to the components. This array can be empty.
  template<typename Type>
  std::vector<std::reference_wrapper<Type>> getComponentsInChildren() {
    if(!_registry->valid(_id)) return {};

    std::vector<std::reference_wrapper<Type>> components {};

    entt::runtime_view view {};
    view.iterate(_registry->storage<Type>());

    for(auto entity : view) {
      components.push_back(_registry->get<Type>(entity));
    }

    return components;
  }

  /// @brief Gets a component on this GameObject's parent.
  /// @tparam Type type of the component to get.
  /// @return the component if it exists, nullptr otherwise.
  template<typename Type>
  Type* getComponentInParent() {
    if(!_registry->valid(_id)) return nullptr;

    const ECS::ObjectData& data = _registry->get<ECS::ObjectData>(_id);

    if(data.parent != entt::null && _registry->all_of<Type>(data.parent)) {
      return &_registry->get<Type>(data.parent);
    }

    return nullptr;
  }

  /// @brief Checks if this GameObject has a tag.
  /// @param tag a tag to check.
  /// @return true if this GameObject has the tag, false otherwise.
  bool hasTag(std::string tag) const;

  /// @brief Adds a tag on this GameObject.
  /// @param tag a tag to add.
  void addTag(std::string tag) const;

  /// @brief Removes a tag from this GameObject, if it exists.
  /// @param tag a tag to remove.
  void removeTag(std::string tag) const;

  /// @brief Checks if this GameObject is active.
  /// @return true if this GameObject is active, false otherwise.
  bool isActive() const;

  /// @brief Sets this GameObject's active state.
  /// @param isActive state to set to this GameObject.
  void setActive(bool isActive) const;

  /// @brief Gets this GameObject's id.
  /// @return this GameObject's id.
  entt::entity getId() const;

  /// @brief Checks if this GameObject has a parent, other than the World he's in.
  /// @return true if this GameObject has a parent, false otherwise.
  bool hasParent() const;

  /// @brief Sets this GameObject's parent.
  /// @param parent the new parent.
  void setParent(GameObject& parent) const;

  /// @brief Removes this GameObject's parent.
  void removeParent() const;

  /// @brief Gets how many children this GameObject has.
  /// @return the amount of children.
  int getChildrenCount() const;

  /// @brief Checks if an object if this GameObject's child.
  /// @param object an object to check.
  /// @return true if the object is this GameObject's child, false otherwise.
  bool isChild(const GameObject& object) const;

  /// @brief Adds a child to this GameObject.
  /// @param child a child to add.
  void addChild(GameObject& child) const;

  /// @brief Removes a child from this GameObject.
  /// @param child a child to remove.
  void removeChild(const GameObject& child) const;

  /// @brief Gets a child at an index from this GameObject.
  /// @param index the index of the child.
  /// @return the child if it exists, nullptr otherwise.
  GameObject* getChild(uint32_t index) const;

  /// @brief Creates a GameObject primitive.
  /// @return a GameObject.
  static GameObject createPrimitive(World& world, GameObjectPrimitives primitive,
    std::shared_ptr<Material> material = nullptr);

  // static GameObject find();
  // static GameObject findWithTag();
  // static std::vector<GameObject> findGameObjectsWithTag();

  bool operator==(const GameObject* obj) const { return _id == obj->_id; }
  bool operator==(const GameObject& obj) const { return _id == obj._id; }
  bool operator!=(const GameObject* obj) const { return !(*this == obj); }
  bool operator!=(const GameObject& obj) const { return !(*this == obj); }
};