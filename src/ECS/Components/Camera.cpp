#include "Camera.h"

#include "../GameObject.h"
#include "Transform.h"

Camera::Camera() {
  transform = gameObject->getComponent<Transform>();
  updateView();
}

void Camera::updateView() { view = glm::inverse(transform->getModelMatrix(WORLD)); }

void Camera::updateProjectionPerspective(float fovY, float aspect, float zNear, float zFar) {
  projectionType = PERSPECTIVE;
  projection = glm::perspective(fovY, aspect, zNear, zFar);
  projection[1][1] *= -1;
}

void Camera::updateProjectionOrthographic(float left, float right, float bottom, float top,
  float zNear, float zFar) {
  projectionType = ORTHOGRAPHIC;
  projection = glm::ortho(left, right, bottom, top, zNear, zFar);
}