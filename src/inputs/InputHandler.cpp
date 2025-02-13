#include "InputHandler.h"

#include "ECS/ECS.h"
#include "utils/Utils.h"
#include "utils/Time.h"
#include "events/EventHandler.h"

bool InputHandler::getButton(SDL_Gamepad* gamepad, SDL_GamepadButton button) {
  return SDL_GetGamepadButton(gamepad, button);
}

float InputHandler::getAxis(SDL_Gamepad* gamepad, SDL_GamepadAxis axis) {
  float ret = SDL_GetGamepadAxis(gamepad, axis) / (float)SDL_JOYSTICK_AXIS_MAX;
  if (axis | SDL_GAMEPAD_AXIS_LEFT_TRIGGER | SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) {
    glm::clamp(ret, 0.f, 1.f);
  }
  else {
    glm::clamp(ret, -1.f, 1.f);
  }
  if (-JOYSTICK_DEADZONE < ret && ret < JOYSTICK_DEADZONE) return 0;
  return ret;
}

vec2 InputHandler::getJoystick(SDL_Gamepad* gamepad, Joystick joystick) {
  switch (joystick) {
    case LEFT:
      return {
        getAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX),
        getAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY)
      };
    case RIGHT:
      return {
        getAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX),
        getAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY)
      };
    default:
      return { 0,0 };
  }
}

float InputHandler::getTriggerRange(SDL_Gamepad* gamepad) {
  return getAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) - getAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
}

vec2 InputHandler::getTriggerVec2(SDL_Gamepad* gamepad) {
  return {
    getAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER),
    getAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
  };
}

void InputHandler::init() {
  EventHandler::keyboardAdded += [](SDL_KeyboardDeviceEvent e) {
    registerKeyboard(e.which);
  };
  EventHandler::keyboardRemoved += [](SDL_KeyboardDeviceEvent e) {
    forgetKeyboard(e.which);
  };

  int keyboardCount = 0;
  SDL_KeyboardID* keyboards = SDL_GetKeyboards(&keyboardCount);
  if(keyboards)
    registerKeyboard(keyboards[0]);
}

void InputHandler::registerPlayerController(PlayerController* controller) {
  auto it = std::find(_playerControllers.begin(), _playerControllers.end(), controller);
  if (it != _playerControllers.end()) return;

  _playerControllers.push_back(controller);

  if (_unusedKeyboards.size() > 0) {
    controller->keyboard = _unusedKeyboards.at(0);
    _unusedKeyboards.pop_front();
  }
  if (_unusedMice.size() > 0) {
    controller->mouse = _unusedMice.at(0);
    _unusedMice.pop_front();
  }
  if (_unusedGamepads.size() > 0 && !controller->usesKeyboard() && !controller->usesMouse()) {
    controller->gamepad = _unusedGamepads.at(0);
    _unusedGamepads.pop_front();
  }
}

void InputHandler::forgetPlayerController(PlayerController* controller) {
  auto it = std::find(_playerControllers.begin(), _playerControllers.end(), controller);
  if (it == _playerControllers.end()) return;

  if ((*it)->usesKeyboard()) {
    _unusedKeyboards.push_back((*it)->keyboard);
    (*it)->keyboard = nullKeyboard;
  }
  if ((*it)->usesMouse()) {
    _unusedMice.push_back((*it)->mouse);
    (*it)->mouse = nullMouse;
  }
  if ((*it)->usesGamepad()) {
    _unusedGamepads.push_back((*it)->gamepad);
    (*it)->gamepad = nullptr;
  }

  _playerControllers.erase(it);
}

void InputHandler::registerKeyboard(SDL_KeyboardID id) {
  auto it = std::find_if(_playerControllers.begin(), _playerControllers.end(), [](PlayerController* pc) {
    return !pc->usesKeyboard() && !pc->usesGamepad();
  });
  if (it != _playerControllers.end()) {
    (*it)->keyboard = id;
  }
  else {
    _unusedKeyboards.push_back(id);
  }
}

void InputHandler::forgetKeyboard(SDL_KeyboardID id) {
  auto it0 = std::find(_unusedKeyboards.begin(), _unusedKeyboards.end(), id);
  if (it0 != _unusedKeyboards.end()) {
    _unusedKeyboards.erase(it0);
    return;
  }

  auto it1 = std::find_if(_playerControllers.begin(), _playerControllers.end(), [&id](PlayerController* pc) {
    return pc->keyboard == id;
  });
  if (it1 != _playerControllers.end()) {
    (*it1)->keyboard = nullKeyboard;
  }
}

void InputHandler::registerMouse(SDL_MouseID id) {
  auto it = std::find_if(_playerControllers.begin(), _playerControllers.end(), [](PlayerController* pc) {
    return !pc->usesMouse() && !pc->usesGamepad();
  });
  if (it != _playerControllers.end()) {
    (*it)->mouse = id;
  }
  else {
    _unusedMice.push_back(id);
  }
}

void InputHandler::forgetMouse(SDL_MouseID id) {
  auto it0 = std::find(_unusedMice.begin(), _unusedMice.end(), id);
  if (it0 != _unusedMice.end()) {
    _unusedMice.erase(it0);
    return;
  }

  auto it1 = std::find_if(_playerControllers.begin(), _playerControllers.end(), [&id](PlayerController* pc) {
    return pc->mouse == id;
  });
  if (it1 != _playerControllers.end()) {
    (*it1)->mouse = nullMouse;
  }
}

void InputHandler::registerGamepad(SDL_JoystickID id) {
  auto it = std::find_if(_playerControllers.begin(), _playerControllers.end(), [](PlayerController* pc) {
    return !pc->hasInputDevice();
  });

  SDL_Gamepad* gamepad = SDL_OpenGamepad(id);
  SDL_CHECK(gamepad);

  if (it != _playerControllers.end()) {
    (*it)->gamepad = gamepad;
  }
  else {
    _unusedGamepads.push_back(gamepad);
  }
}

void InputHandler::forgetGamepad(SDL_JoystickID id) {
  SDL_Gamepad* gamepad = SDL_GetGamepadFromID(id);

  auto it0 = std::find(_unusedGamepads.begin(), _unusedGamepads.end(), gamepad);
  if (it0 != _unusedGamepads.end()) {
    SDL_CloseGamepad((*it0));
    _unusedGamepads.erase(it0);
    return;
  }

  auto it1 = std::find_if(_playerControllers.begin(), _playerControllers.end(), [&gamepad](PlayerController* pc) {
    return pc->gamepad == gamepad;
  });
  if (it1 != _playerControllers.end()) {
    SDL_CloseGamepad((*it1)->gamepad);
    (*it1)->gamepad = nullptr;
  }
}

void InputHandler::processInputs() {
  for (auto& playerController : _playerControllers) {
    if (playerController->usesKeyboard()) {
      int numKeys = 0;
      const bool* keyboardStates = SDL_GetKeyboardState(&numKeys);

      vec3 movement{};

      if (keyboardStates[SDL_SCANCODE_D]) {
        movement.x += 1;
      }
      if (keyboardStates[SDL_SCANCODE_A]) {
        movement.x -= 1;
      }
      if (keyboardStates[SDL_SCANCODE_Q]) {
        movement.y -= 1;
      }
      if (keyboardStates[SDL_SCANCODE_E]) {
        movement.y += 1;
      }
      if (keyboardStates[SDL_SCANCODE_S]) {
        movement.z -= 1;
      }
      if (keyboardStates[SDL_SCANCODE_W]) {
        movement.z += 1;
      }

      movement *= playerController->movementSpeed * Time::deltaTime;
      playerController->gameObject->getComponent<Transform>()->translate(movement);

      vec3 rotation{};
      if (keyboardStates[SDL_SCANCODE_LEFT]) {
        rotation.y += 1;
      }
      if (keyboardStates[SDL_SCANCODE_RIGHT]) {
        rotation.y -= 1;
      }
      if (keyboardStates[SDL_SCANCODE_DOWN]) {
        rotation.x -= 1;
      }
      if (keyboardStates[SDL_SCANCODE_UP]) {
        rotation.x += 1;
      }

      rotation *= playerController->rotationSpeed * Time::deltaTime;
      playerController->gameObject->getComponent<Transform>()->rotate(rotation);
    }

    //if (playerController->gamepad) {

    //  vec2 val2 = getJoystick(playerController->gamepad, LEFT);
    //  if (glm::length(val2) > 0) std::cout << "LEFT JOYSTICK" << val2 << "\n";

    //  val2 = getJoystick(playerController->gamepad, RIGHT);
    //  if (glm::length(val2) > 0) std::cout << "RIGHT JOYSTICK" << val2 << "\n";

    //  float val3 = getTriggerRange(playerController->gamepad);
    //  if (val3 != 0) std::cout << "TRIGGERS" << val2 << "\n";

    //  // ------------------------------------------------------------------

    //   vec3 movement = {
    //     SDL_GetGamepadAxis(playerController->gamepad, SDL_GAMEPAD_AXIS_LEFTX) / (float)SDL_JOYSTICK_AXIS_MAX,
    //     SDL_GetGamepadButton(playerController->gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER) - SDL_GetGamepadButton(playerController->gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER),
    //     SDL_GetGamepadAxis(playerController->gamepad, SDL_GAMEPAD_AXIS_LEFTY) / (float)SDL_JOYSTICK_AXIS_MAX
    //   };
    //   if (movement.x < SDL_JOYSTICK_AXIS_MAX * JOYSTICK_DEADZONE)  movement.x = 0;
    //   if (movement.z < SDL_JOYSTICK_AXIS_MAX * JOYSTICK_DEADZONE)  movement.z = 0;

    //   //movement = glm::normalize(movement);
    //   movement *= playerController->movementSpeed * Time::deltaTime;

    //   //std::cout << movement << "\n";
    //   playerController->gameObject->getComponent<Transform>()->position += movement;
    //}
  }
}
