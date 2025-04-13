#pragma once

#include "utils/Utils.h"

#include <deque>
#include <map>

struct PlayerController;

enum KeyState { DOWN, MAINTAINED, UP };

class InputHandler {
private:
  static inline const float JOYSTICK_DEADZONE = 0.05f;
  static inline std::vector<PlayerController*> _playerControllers {};
  static inline std::vector<PlayerController*> _unusablePlayerControllers {};

  static inline std::deque<SDL_KeyboardID> _unusedKeyboards {};
  static inline std::deque<SDL_MouseID> _unusedMice {};
  static inline std::deque<SDL_Gamepad*> _unusedGamepads {};

  enum Joystick { LEFT, RIGHT };

  /// @brief Get the current state of a button on a gamepad.
  /// @param gamepad a gamepad.
  /// @param button a button index (one of the SDL_GamepadButton values).
  /// @returns true if the button is pressed, false otherwise.
  static bool getButton(SDL_Gamepad* gamepad, SDL_GamepadButton button);

  /// @brief Get the current state of an axis on a gamepad.
  ///
  /// For triggers, the range is [0, 1], 0 being released and 1 being pressed.
  ///
  /// For joysticks, the range is [-1, 1], -1 being up/left and 1 being down/right.
  ///
  /// @param gamepad a gamepad.
  /// @param button an axis index (one of the SDL_GamepadAxis values).
  /// @return a value in one of the two ranges, depending on the axis.
  static float getAxis(SDL_Gamepad* gamepad, SDL_GamepadAxis axis);

  /// @brief Get the normalized (x,y) axes of a joystick.
  /// @param gamepad a gamepad.
  /// @param axis a joystick, either LEFT or RIGHT.
  /// @return the normalized (x,y) axes.
  static vec2 getJoystick(SDL_Gamepad* gamepad, Joystick joystick);

public:
  /// @brief Sets up input related events and queries keyboards, mice and controllers
  static void init();

  /// @brief Assigns a keyboard to a player controller if one is free.
  ///
  /// If a keyboard is assigned, also assigns a mouse if one is free.
  ///
  /// If no keyboard was assigned, assigns a gamepad if one is free.
  ///
  /// Otherwise, controller is put on a waiting list and will be assigned one of
  /// the three when they're either connected or freed.
  ///
  /// @param controller a controller.
  static void registerPlayerController(PlayerController* controller);

  /// @brief Forgets player controller and retrieves his keyboard, mouse and
  /// gamepad.
  ///
  /// If possible, the keyboard, mouse and gamepad will be reassigned to other
  /// player controllers who needs it.
  ///
  /// @param controller a controller.
  static void forgetPlayerController(PlayerController* controller);

  /// @brief Assigns keyboard to a player controller if needed.
  ///
  /// Puts keyboard in waiting list otherwise.
  ///
  /// @param id a keyboard id.
  static void registerKeyboard(SDL_KeyboardID id);

  /// @brief Forgets keyboard.
  /// @param id a keyboard id.
  static void forgetKeyboard(SDL_KeyboardID id);

  /// @brief Assigns mouse to a player controller if needed.
  ///
  /// Puts mouse in waiting list otherwise.
  ///
  /// @param id a mouse id.
  static void registerMouse(SDL_MouseID id);

  /// @brief Forgets a mouse.
  /// @param id a mouse id.
  static void forgetMouse(SDL_MouseID id);

  /// @brief Assigns gamepad to a player controller if needed.
  ///
  /// Puts gamepad in waiting list otherwise.
  ///
  /// @param id a gamepad id.
  static void registerGamepad(SDL_JoystickID id);

  /// @brief Forgets a gamepad.
  /// @param id a gamepad id.
  static void forgetGamepad(SDL_JoystickID id);

  /// @brief Processes all inputs.
  static void processInputs();
};