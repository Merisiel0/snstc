#include "Transform.h"

#include "ECS/GameObject.h"
#include "graphics/vulkan/Buffer.h"

Transform::Transform(vec3 position, quat rotation, vec3 scale) :
    position {position}, rotation {rotation}, scale {scale} {}

mat4 Transform::getTranslationMatrix(Space space) const {
  return glm::translate(mat4(1.0f), getPosition(space));
}

mat4 Transform::getRotationMatrix(Space space) const {
  quat rot = getRotation(space);
  return glm::rotate(mat4(1.0f), angle(rot), axis(rot));
}

mat4 Transform::getScaleMatrix(Space space) const {
  return glm::scale(mat4(1.0f), getScale(space));
}

mat4 Transform::getModelMatrix() const {
  vec3 pos = position;
  quat rot = rotation;
  vec3 scl = scale;

  ECS::ObjectData* data = gameObject->getComponent<ECS::ObjectData>();
  while(data->parent && data->parent->hasComponent<Transform>()) {
    Transform* parentTransform = data->parent->getComponent<Transform>();
    pos += parentTransform->position;
    rot = parentTransform->rotation * rot;
    scl *= parentTransform->scale;

    data = data->parent->getComponent<ECS::ObjectData>();
  }

  mat4 mat = mat4(1.0f);
  mat = glm::translate(mat, pos);
  mat = glm::rotate(mat, angle(rot), axis(rot));
  mat = glm::scale(mat, scl);

  return mat;
}

vec3 Transform::getPosition(Space space) const {
  if(space == SELF) { return position; }

  ECS::ObjectData* data = gameObject->getComponent<ECS::ObjectData>();
  if(data->parent) {
    return position + data->parent->getComponent<Transform>()->getPosition(WORLD);
  }
  return position;
}

quat Transform::getRotation(Space space) const {
  if(space == SELF) { return rotation; }

  ECS::ObjectData* data = gameObject->getComponent<ECS::ObjectData>();
  if(data->parent) {
    return data->parent->getComponent<Transform>()->getRotation(WORLD) * rotation;
  }
  return rotation;
}

vec3 Transform::getScale(Space space) const {
  if(space == SELF) { return scale; }

  ECS::ObjectData* data = gameObject->getComponent<ECS::ObjectData>();
  if(data->parent) {
    return scale * data->parent->getComponent<Transform>()->getScale(WORLD);
  }
  return scale;
}

void Transform::getDirections(vec3& right, vec3& up, vec3& forward, Space space) const {
  if(space == WORLD) {
    right = {1, 0, 0};
    up = {0, 1, 0};
    forward = {0, 0, 1};
  } else {
    right = this->right();
    up = this->up();
    forward = this->forward();
  }
}

void Transform::translate(vec3 movement, Space space) {
  vec3 right, up, forward;
  getDirections(right, up, forward, space);

  position += right * movement.x + up * movement.y + forward * movement.z;
}

void Transform::translate(float x, float y, float z, Space space) {
  this->translate({x, y, z}, space);
}

void Transform::rotate(vec3 angles, Space space) {
  vec3 right, up, forward;
  getDirections(right, up, forward, space);

  rotation = glm::rotate(rotation, angles.x, right);
  rotation = glm::rotate(rotation, angles.y, up);
  rotation = glm::rotate(rotation, angles.z, forward);
}

void Transform::rotate(float x, float y, float z, Space space) { this->rotate({x, y, z}, space); }

vec3 Transform::right() const {
  mat4 mat = getModelMatrix();
  return {mat[0].x, mat[0].y, mat[0].z};
}

vec3 Transform::left() const { return right() * -1.0f; }

vec3 Transform::up() const {
  mat4 mat = getModelMatrix();
  return {mat[1].x, mat[1].y, mat[1].z};
}

vec3 Transform::down() const { return up() * -1.0f; }

vec3 Transform::forward() const { return backward() * -1.0f; }

vec3 Transform::backward() const {
  mat4 mat = getModelMatrix();
  return {mat[2].x, mat[2].y, mat[2].z};
}
