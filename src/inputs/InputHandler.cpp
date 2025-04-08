#include "InputHandler.h"

#include "ECS/ECS.h"
#include "events/EventHandler.h"
#include "utils/Time.h"
#include "utils/Utils.h"

/**
 * @brief Get the current state of a button on a gamepad.
 *
 * @param gamepad a gamepad.
 * @param button a button index (one of the SDL_GamepadButton values).
 * @returns true if the button is pressed, false otherwise.
 *
 */
bool InputHandler::getButton(SDL_Gamepad* gamepad, SDL_GamepadButton button) {
  return SDL_GetGamepadButton(gamepad, button);
}

/**
 * @brief Get the current state of an axis on a gamepad.
 *
 * For triggers, the range is [0, 1], 0 being released and 1 being pressed.
 *
 * For joysticks, the range is [-1, 1], -1 being up/left and 1 being down/right.
 *
 * @param gamepad a gamepad.
 * @param axis an axis index (one of the SDL_GamepadAxis values).
 * @return a value in one of the two ranges, depending on the axis.
 */
float InputHandler::getAxis(SDL_Gamepad* gamepad, SDL_GamepadAxis axis) {
  float ret = SDL_GetGamepadAxis(gamepad, axis) / (float) SDL_JOYSTICK_AXIS_MAX;
  if(axis | SDL_GAMEPAD_AXIS_LEFT_TRIGGER | SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) {
    ret = glm::clamp(ret, 0.f, 1.f);
  } else {
    ret = glm::clamp(ret, -1.f, 1.f);
  }
  if(-JOYSTICK_DEADZONE < ret && ret < JOYSTICK_DEADZONE) return 0;
  return ret;
}

/**
 * @brief Get the normalized (x,y) axes of a joystick.
 *
 * @param gamepad a gamepad.
 * @param joystick a joystick, either LEFT or RIGHT.
 * @return the normalized (x,y) axes.
 */
vec2 InputHandler::getJoystick(SDL_Gamepad* gamepad, Joystick joystick) {
  vec2 axes = {0, 0};

  switch(joystick) {
    case LEFT:
      axes = {getAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX), getAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY)};
    case RIGHT:
      axes = {getAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX), getAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY)};
  }

  return normalize(axes);
}

/**
 * @brief Sets up input related events and queries keyboards, mice and
 * controllers
 *
 */
void InputHandler::init() {
  // setup keyboard events
  EventHandler::keyboardAdded += [](SDL_KeyboardDeviceEvent e) { registerKeyboard(e.which); };
  EventHandler::keyboardRemoved += [](SDL_KeyboardDeviceEvent e) { forgetKeyboard(e.which); };

  // setup mouse events
  EventHandler::mouseAdded += [](SDL_MouseDeviceEvent e) { registerMouse(e.which); };
  EventHandler::mouseRemoved += [](SDL_MouseDeviceEvent e) { forgetMouse(e.which); };

  // setup gamepad events
  EventHandler::gamepadAdded += [](SDL_GamepadDeviceEvent e) { registerGamepad(e.which); };
  EventHandler::gamepadRemoved += [](SDL_GamepadDeviceEvent e) { forgetGamepad(e.which); };

  // setup keyboards
  int keyboardCount = 0;
  SDL_KeyboardID* keyboards = SDL_GetKeyboards(&keyboardCount);
  SDL_CHECK(keyboards);

  for(int i = 0; i < keyboardCount; i++) {
    registerKeyboard(keyboards[i]);
  }

  SDL_free(keyboards);

  // setup mice
  int miceCount = 0;
  SDL_MouseID* mice = SDL_GetMice(&miceCount);
  SDL_CHECK(mice);

  for(int i = 0; i < miceCount; i++) {
    registerMouse(mice[i]);
  }

  SDL_free(mice);

  // setup controllers
  int gamepadCount = 0;
  SDL_JoystickID* gamepads = SDL_GetGamepads(&gamepadCount);
  SDL_CHECK(gamepads);

  for(int i = 0; i < gamepadCount; i++) {
    registerGamepad(gamepads[i]);
  }

  SDL_free(gamepads);
}

/**
 * @brief Assigns a keyboard to a player controller if one is free.
 *
 * If a keyboard is assigned, also assigns a mouse if one is free.
 *
 * If no keyboard was assigned, assigns a gamepad if one is free.
 *
 * Otherwise, controller is put on a waiting list and will be assigned one of
 * the three when they're either connected or freed.
 *
 * @param controller a controller.
 */
void InputHandler::registerPlayerController(PlayerController* controller) {
  auto it = std::find(_playerControllers.begin(), _playerControllers.end(), controller);
  if(it != _playerControllers.end()) return;

  if(_unusedKeyboards.size() > 0) {
    controller->keyboard = _unusedKeyboards.front();
    _unusedKeyboards.pop_front();
    _playerControllers.push_back(controller);
  }
  if(controller->usesKeyboard() && _unusedMice.size() > 0) {
    controller->mouse = _unusedMice.front();
    _unusedMice.pop_front();
    return;
  }
  if(_unusedGamepads.size() > 0 && !controller->usesKeyboard() && !controller->usesMouse()) {
    controller->gamepad = _unusedGamepads.front();
    _unusedGamepads.pop_front();
    _playerControllers.push_back(controller);
    return;
  }

  _unusablePlayerControllers.push_back(controller);
}

/**
 * @brief Forgets player controller and retrieves his keyboard, mouse and
 * gamepad.
 *
 * If possible, the keyboard, mouse and gamepad will be reassigned to other
 * player controllers who needs it.
 *
 * @param controller a controller.
 */
void InputHandler::forgetPlayerController(PlayerController* controller) {
  auto it = std::find(_playerControllers.begin(), _playerControllers.end(), controller);
  if(it == _playerControllers.end()) return;

  // retrieve keyboard
  if((*it)->usesKeyboard()) {
    _unusedKeyboards.push_back((*it)->keyboard);
    (*it)->keyboard = nullKeyboard;
  }

  // retrieve mouse
  if((*it)->usesMouse()) {
    _unusedMice.push_back((*it)->mouse);
    (*it)->mouse = nullMouse;
  }

  // retrieve gamepad
  if((*it)->usesGamepad()) {
    _unusedGamepads.push_back((*it)->gamepad);
    (*it)->gamepad = nullptr;
  }

  _playerControllers.erase(it);

  // reassigns keyboard, mouse and gamepad if possible
  if(_unusablePlayerControllers.size() == 0) return;

  PlayerController* playerController = _unusablePlayerControllers.back();
  _unusablePlayerControllers.pop_back();

  registerPlayerController(playerController);
}

/**
 * @brief Assigns keyboard to a player controller if needed.
 *
 * Puts keyboard in waiting list otherwise.
 *
 * @param id a keyboard id.
 */
void InputHandler::registerKeyboard(SDL_KeyboardID id) {
  auto it = std::find_if(_playerControllers.begin(), _playerControllers.end(),
    [](PlayerController* pc) { return !pc->usesKeyboard() && !pc->usesGamepad(); });
  if(it != _playerControllers.end()) {
    (*it)->keyboard = id;
  } else {
    _unusedKeyboards.push_back(id);
  }
}

/**
 * @brief Forgets keyboard.
 *
 * @param id a keyboard id.
 */
void InputHandler::forgetKeyboard(SDL_KeyboardID id) {
  auto it0 = std::find(_unusedKeyboards.begin(), _unusedKeyboards.end(), id);
  if(it0 != _unusedKeyboards.end()) {
    _unusedKeyboards.erase(it0);
    return;
  }

  auto it1 = std::find_if(_playerControllers.begin(), _playerControllers.end(),
    [&id](PlayerController* pc) { return pc->keyboard == id; });
  if(it1 != _playerControllers.end()) {
    (*it1)->keyboard = nullKeyboard;

    if(!(*it1)->hasInputDevice()) {
      _unusablePlayerControllers.push_back((*it1));
      _playerControllers.erase(it1);
    }
  }
}

/**
 * @brief Assigns mouse to a player controller if needed.
 *
 * Puts mouse in waiting list otherwise.
 *
 * @param id a mouse id.
 */
void InputHandler::registerMouse(SDL_MouseID id) {
  auto it = std::find_if(_playerControllers.begin(), _playerControllers.end(),
    [](PlayerController* pc) { return !pc->usesMouse() && !pc->usesGamepad(); });
  if(it != _playerControllers.end()) {
    (*it)->mouse = id;
  } else {
    _unusedMice.push_back(id);
  }
}

/**
 * @brief Forgets a mouse.
 * 
 * @param id a mouse id.
 */
void InputHandler::forgetMouse(SDL_MouseID id) {
  auto it0 = std::find(_unusedMice.begin(), _unusedMice.end(), id);
  if(it0 != _unusedMice.end()) {
    _unusedMice.erase(it0);
    return;
  }

  auto it1 = std::find_if(_playerControllers.begin(), _playerControllers.end(),
    [&id](PlayerController* pc) { return pc->mouse == id; });
  if(it1 != _playerControllers.end()) {
    (*it1)->mouse = nullMouse;

    if(!(*it1)->hasInputDevice()) {
      _unusablePlayerControllers.push_back((*it1));
      _playerControllers.erase(it1);
    }
  }
}

/**
 * @brief Assigns gamepad to a player controller if needed.
 *
 * Puts gamepad in waiting list otherwise.
 *
 * @param id a gamepad id.
 */
void InputHandler::registerGamepad(SDL_JoystickID id) {
  auto it = std::find_if(_playerControllers.begin(), _playerControllers.end(),
    [](PlayerController* pc) { return !pc->hasInputDevice(); });

  SDL_Gamepad* gamepad = SDL_OpenGamepad(id);
  SDL_CHECK(gamepad);

  if(it != _playerControllers.end()) {
    (*it)->gamepad = gamepad;
  } else {
    _unusedGamepads.push_back(gamepad);
  }
}

/**
 * @brief Forgets a gamepad.
 * 
 * @param id a gamepad id.
 */
void InputHandler::forgetGamepad(SDL_JoystickID id) {
  SDL_Gamepad* gamepad = SDL_GetGamepadFromID(id);

  auto it0 = std::find(_unusedGamepads.begin(), _unusedGamepads.end(), gamepad);
  if(it0 != _unusedGamepads.end()) {
    SDL_CloseGamepad((*it0));
    _unusedGamepads.erase(it0);
    return;
  }

  auto it1 = std::find_if(_playerControllers.begin(), _playerControllers.end(),
    [&gamepad](PlayerController* pc) { return pc->gamepad == gamepad; });
  if(it1 != _playerControllers.end()) {
    SDL_CloseGamepad((*it1)->gamepad);
    (*it1)->gamepad = nullptr;

    if(!(*it1)->hasInputDevice()) {
      _unusablePlayerControllers.push_back((*it1));
      _playerControllers.erase(it1);
    }
  }
}

/**
 * @brief Processes all inputs. Under construction.
 * 
 */
void InputHandler::processInputs() {
  for(auto& playerController : _playerControllers) {
    if(playerController->usesKeyboard()) {
      int numKeys = 0;
      const bool* keyboardStates = SDL_GetKeyboardState(&numKeys);

      vec3 movement {};

      if(keyboardStates[SDL_SCANCODE_D]) { movement.x += 1; }
      if(keyboardStates[SDL_SCANCODE_A]) { movement.x -= 1; }
      if(keyboardStates[SDL_SCANCODE_Q]) { movement.y -= 1; }
      if(keyboardStates[SDL_SCANCODE_E]) { movement.y += 1; }
      if(keyboardStates[SDL_SCANCODE_S]) { movement.z -= 1; }
      if(keyboardStates[SDL_SCANCODE_W]) { movement.z += 1; }

      movement *= playerController->movementSpeed * Time::deltaTime;
      playerController->gameObject->getComponent<Transform>()->translate(movement);

      vec3 rotation {};
      if(keyboardStates[SDL_SCANCODE_LEFT]) { rotation.y += 1; }
      if(keyboardStates[SDL_SCANCODE_RIGHT]) { rotation.y -= 1; }
      if(keyboardStates[SDL_SCANCODE_DOWN]) { rotation.x -= 1; }
      if(keyboardStates[SDL_SCANCODE_UP]) { rotation.x += 1; }

      rotation *= playerController->rotationSpeed * Time::deltaTime;
      playerController->gameObject->getComponent<Transform>()->rotate(rotation);
    }

    // if (playerController->gamepad) {

    //  vec2 val2 = getJoystick(playerController->gamepad, LEFT);
    //  if (glm::length(val2) > 0) std::cout << "LEFT JOYSTICK" << val2 << "\n";

    //  val2 = getJoystick(playerController->gamepad, RIGHT);
    //  if (glm::length(val2) > 0) std::cout << "RIGHT JOYSTICK" << val2 <<
    //  "\n";

    //  float val3 = getTriggerRange(playerController->gamepad);
    //  if (val3 != 0) std::cout << "TRIGGERS" << val2 << "\n";

    //  // ------------------------------------------------------------------

    //   vec3 movement = {
    //     SDL_GetGamepadAxis(playerController->gamepad, SDL_GAMEPAD_AXIS_LEFTX)
    //     / (float)SDL_JOYSTICK_AXIS_MAX,
    //     SDL_GetGamepadButton(playerController->gamepad,
    //     SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER) -
    //     SDL_GetGamepadButton(playerController->gamepad,
    //     SDL_GAMEPAD_BUTTON_LEFT_SHOULDER),
    //     SDL_GetGamepadAxis(playerController->gamepad, SDL_GAMEPAD_AXIS_LEFTY)
    //     / (float)SDL_JOYSTICK_AXIS_MAX
    //   };
    //   if (movement.x < SDL_JOYSTICK_AXIS_MAX * JOYSTICK_DEADZONE)  movement.x
    //   = 0; if (movement.z < SDL_JOYSTICK_AXIS_MAX * JOYSTICK_DEADZONE)
    //   movement.z = 0;

    //   //movement = glm::normalize(movement);
    //   movement *= playerController->movementSpeed * Time::deltaTime;

    //   //std::cout << movement << "\n";
    //   playerController->gameObject->getComponent<Transform>()->position +=
    //   movement;
    //}
  }
}
