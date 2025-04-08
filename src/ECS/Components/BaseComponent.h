#pragma once

class GameObject;

namespace ECS {
  struct BaseComponent {
  public:
    static inline GameObject* gameObjectBuffer {nullptr};
    GameObject* gameObject {nullptr};

  protected:
    BaseComponent() {
      gameObject = gameObjectBuffer;
      gameObjectBuffer = nullptr;
    };
  };
} // namespace ECS