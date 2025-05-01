#pragma once

#include "../../utils/Math.h"
#include "BaseComponent.h"

#include <memory>

class Buffer;
class CommandBuffer;

enum Space { WORLD, SELF };

struct Transform : public ECS::BaseComponent {
  vec3 position;
  quat rotation;
  vec3 scale;

  Transform(vec3 position, quat rotation, vec3 scale);
  Transform() : Transform({0, 0, 0}, {glm::identity<quat>()}, {1, 1, 1}) {}

  mat4 getTranslationMatrix(Space space) const;
  mat4 getRotationMatrix(Space space) const;
  mat4 getScaleMatrix(Space space) const;

  mat4 getModelMatrix(Space space) const;

  vec3 getPosition(Space space) const;
  quat getRotation(Space space) const;
  vec3 getScale(Space space) const;

  void getDirections(vec3& right, vec3& up, vec3& forward, Space space) const;
  void translate(vec3 movement, Space space);
  void translate(float x, float y, float z, Space space);
  void rotate(vec3 angles, Space space);
  void rotate(float x, float y, float z, Space space);

  vec3 right() const;
  vec3 left() const;
  vec3 up() const;
  vec3 down() const;
  vec3 forward() const;
  vec3 backward() const;
};