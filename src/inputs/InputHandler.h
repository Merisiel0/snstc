#pragma once

#include <deque>
#include <map>

#include "utils/Utils.h"

struct PlayerController;

enum KeyState { DOWN, MAINTAINED, UP };

class InputHandler {
 private:
  static inline const float JOYSTICK_DEADZONE = 0.05f;
  static inline std::vector<PlayerController*> _playerControllers{};
  static inline std::vector<PlayerController*> _unusablePlayerControllers{};

  static inline std::deque<SDL_KeyboardID> _unusedKeyboards{};
  static inline std::deque<SDL_MouseID> _unusedMice{};
  static inline std::deque<SDL_Gamepad*> _unusedGamepads{};

  enum Joystick { LEFT, RIGHT };

  static bool getButton(SDL_Gamepad* gamepad, SDL_GamepadButton button);
  static float getAxis(SDL_Gamepad* gamepad, SDL_GamepadAxis axis);
  static vec2 getJoystick(SDL_Gamepad* gamepad, Joystick joystick);

 public:
  static void init();

  static void registerPlayerController(PlayerController* controller);
  static void forgetPlayerController(PlayerController* controller);

  static void registerKeyboard(SDL_KeyboardID id);
  static void forgetKeyboard(SDL_KeyboardID id);

  static void registerMouse(SDL_MouseID id);
  static void forgetMouse(SDL_MouseID id);

  static void registerGamepad(SDL_JoystickID id);
  static void forgetGamepad(SDL_JoystickID id);

  static void processInputs();
};