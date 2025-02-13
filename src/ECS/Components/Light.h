#pragma once

#include "BaseComponent.h"
#include "../../utils/Utils.h"

struct LightBuffer {
  Color ambiantColor;
  Color color;
  vec3 position;
  vec3 viewPosition;
};

struct AmbiantLight : ECS::BaseComponent {
  // r,g,b make the color, a is the strength of the light
  Color color;

  AmbiantLight(Color color) : color{ color } {}
};

struct Light : ECS::BaseComponent {
  // r,g,b make the color, a is the strength of the light
  Color color;

  Light(Color color) : color{ color } {}
};
