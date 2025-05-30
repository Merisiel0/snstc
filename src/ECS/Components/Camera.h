#pragma once

#include "../../utils/Math.h"
#include "BaseComponent.h"

struct Transform;

struct CameraBuffer {
  mat4 projection;
  mat4 view;
};

enum CameraProjections { PERSPECTIVE, ORTHOGRAPHIC };

struct Camera : public ECS::BaseComponent {
private:
  Transform* transform;

public:
  mat4 view;
  mat4 projection;
  CameraProjections projectionType {PERSPECTIVE};

  void updateView();
  void updateProjectionPerspective(float fovY, float aspect, float zNear, float zFar);
  void updateProjectionOrthographic(float left, float right, float bottom, float top, float zNear,
    float zFar);

  Camera();
  Camera(CameraProjections projectionType) : Camera() { this->projectionType = projectionType; }
};