#pragma once

#include "../../utils/Math.h"
#include "BaseComponent.h"

enum Space { WORLD, SELF };

struct Transform : public ECS::BaseComponent {
  vec3 position;
  quat rotation;
  vec3 scale;

  Transform(vec3 position, quat rotation, vec3 scale) :
      position {position}, rotation {rotation}, scale {scale} {}
  Transform() : Transform({0, 0, 0}, {glm::identity<quat>()}, {1, 1, 1}) {}
  Transform(const Transform& t) : position {t.position}, rotation {t.rotation}, scale {t.scale} {}

  mat4 modelMatrix() const;
  vec3 truePosition() const;

  void getDirections(vec3& right, vec3& up, vec3& forward, Space space = SELF) const;
  void translate(vec3 movement, Space space = SELF);
  void translate(float x, float y, float z, Space space = SELF);
  void rotate(vec3 angles, Space space = SELF);
  void rotate(float x, float y, float z, Space space = SELF);

  vec3 right() const;
  vec3 left() const;
  vec3 up() const;
  vec3 down() const;
  vec3 forward() const;
  vec3 backward() const;
};