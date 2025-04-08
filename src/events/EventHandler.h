#pragma once

#include "SDL3/SDL.h"
#include "utils/Utils.h"

#include <functional>
#include <map>

/* application event callbacks */
typedef void (*QuitEventCallback)(SDL_QuitEvent);

/* keyboard event callbacks */
typedef void (*KeyboardEventCallback)(SDL_KeyboardEvent);
typedef void (*KeyboardDeviceEventCallback)(SDL_KeyboardDeviceEvent);

// typedef void (*MouseMotionEventCallback)(SDL_MouseMotionEvent);
// typedef void (*MouseButtonEventCallback)(SDL_MouseButtonEvent);
// typedef void (*MouseWheelEventCallback)(SDL_MouseWheelEvent);
typedef void (*MouseDeviceEventCallback)(SDL_MouseDeviceEvent);

//typedef void (*ControllerAxisEventCallback)(SDL_ControllerAxisEvent);
//typedef void (*ControllerButtonEventCallback)(SDL_ControllerButtonEvent);
typedef void (*ControllerDeviceEventCallback)(SDL_GamepadDeviceEvent);

// Event is a wrapper for function pointers of T type to be called when SDL_<TYPE>Event U is called.
// Any function pointer can be added, but lambdas and std::functions can't be removed, for they lack the == operator.
template<typename T, typename U>
class Event {
private:
  std::vector<T> _callbacks {};

public:
  Event() {}

  void operator+=(const T& callback) {
    assert(callback == callback);

    _callbacks.push_back(callback);
  }

  void operator-=(T callback) {
    auto it = std::find(_callbacks.begin(), _callbacks.end(), callback);
    if(it != _callbacks.end()) { _callbacks.erase(it); }
  }

  void operator()(U e) {
    for(const auto& callback : _callbacks) {
      callback(e);
    }
  }

  void clearCallbacks() { _callbacks.clear(); }
};

class EventHandler {
public:
  /* application events */
  static inline Event<QuitEventCallback, SDL_QuitEvent> quit {};

  /* keyboard events */
  static inline Event<KeyboardDeviceEventCallback, SDL_KeyboardDeviceEvent> keyboardAdded {};
  static inline Event<KeyboardDeviceEventCallback, SDL_KeyboardDeviceEvent> keyboardRemoved {};
  static inline Event<KeyboardEventCallback, SDL_KeyboardEvent> keyDown {};
  static inline Event<KeyboardEventCallback, SDL_KeyboardEvent> keyUp {};

  static inline Event<MouseDeviceEventCallback, SDL_MouseDeviceEvent> mouseAdded {};
  static inline Event<MouseDeviceEventCallback, SDL_MouseDeviceEvent> mouseRemoved {};
  //static inline Event<MouseMotionEventCallback, SDL_MouseMotionEvent> mouseMotion{};
  //static inline Event<MouseButtonEventCallback, SDL_MouseButtonEvent> mouseButtonDown{};
  //static inline Event<MouseButtonEventCallback, SDL_MouseButtonEvent> mouseButtonUp{};
  //static inline Event<MouseWheelEventCallback, SDL_MouseWheelEvent> mouseWheel{};

  static inline Event<ControllerDeviceEventCallback, SDL_GamepadDeviceEvent> gamepadAdded {};
  static inline Event<ControllerDeviceEventCallback, SDL_GamepadDeviceEvent> gamepadRemoved {};
  //static inline Event<ControllerAxisEventCallback, SDL_ControllerAxisEvent> axisMotion{};
  //static inline Event<ControllerButtonEventCallback, SDL_ControllerButtonEvent> buttonDown{};
  //static inline Event<ControllerButtonEventCallback, SDL_ControllerButtonEvent> buttonUp{};

  static void processEvents();
};