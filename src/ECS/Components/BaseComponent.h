#pragma once

class GameObject;

namespace ECS {
  struct BaseComponent {
  private:
    friend class ::GameObject;
    static inline GameObject* gameObjectBuffer {nullptr};

  protected:
    BaseComponent() {
      gameObject = gameObjectBuffer;
      gameObjectBuffer = nullptr;
    };

  public:
    GameObject* gameObject {nullptr};

    // A component should never be copied.
    BaseComponent(const BaseComponent& other) = delete;
    BaseComponent& operator=(const BaseComponent& other) = delete;

    // A component should never be moved.
    BaseComponent(BaseComponent&& other) = delete;
    BaseComponent& operator=(BaseComponent&& other) = delete;
  };
} // namespace ECS