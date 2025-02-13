#pragma once

#include "BaseComponent.h"
#include "utils/Utils.h"
#include <array>
#include <variant>

struct Transform;

/*

Buttons/keys and axes

keys
mouse axes
mouse buttons
gamepad axes
gamepad buttons

*/

enum ActionType {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_DOWN,
  MOVE_UP,
  MOVE_BACK,
  MOVE_FRONT,
  PITCH_CLOCKWISE, // x
  PITCH_COUNTER_CLOCKWISE,
  YAW_CLOCKWISE, // y
  YAW_COUNTER_CLOCKWISE,
  ROLL_CLOCKWISE, // z
  ROLL_COUNTER_CLOCKWISE,
  MAX_ACTION_TYPE
};

static const SDL_KeyboardID nullKeyboard = std::numeric_limits<SDL_KeyboardID>::max();
static const SDL_MouseID nullMouse = std::numeric_limits<SDL_MouseID>::max();

struct PlayerController : ECS::BaseComponent {
  SDL_KeyboardID keyboard{ nullKeyboard };
  SDL_MouseID mouse{ nullMouse };
  SDL_Gamepad* gamepad{ nullptr };

  float movementSpeed;
  float rotationSpeed;

  PlayerController(float movementSpeed, float rotationSpeed);
  ~PlayerController();

  bool usesKeyboard() const { return keyboard != nullKeyboard; }
  bool usesMouse() const { return mouse != nullMouse; }
  bool usesGamepad() const { return gamepad; }
  bool hasInputDevice() const { return usesKeyboard() || usesMouse() || usesGamepad(); }
};