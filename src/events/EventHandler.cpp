#include "EventHandler.h"

#include "inputs/InputHandler.h"

void EventHandler::processEvents() {
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0) {
    switch(e.type) {
      /* application events */
      case SDL_EVENT_QUIT:
        quit(e.quit);
        break;
        //case SDL_EVENT_LOCALE_CHANGED:
        //  break;
        //case SDL_EVENT_SYSTEM_THEME_CHANGED:
        //  break;

        /* display events */
        // TODO

        /* window events */
        // TODO

      /* keyboard events */
      case SDL_EVENT_KEY_DOWN:
        keyDown(e.key);
        break;
      case SDL_EVENT_KEY_UP:
        keyUp(e.key);
        break;
      case SDL_EVENT_KEYBOARD_ADDED:
        keyboardAdded(e.kdevice);
        break;
      case SDL_EVENT_KEYBOARD_REMOVED:
        keyboardRemoved(e.kdevice);
        break;

        /* mouse events */
        //case SDL_EVENT_MOUSE_MOTION:
        //  mouseMotion(e.motion);
        //  break;
        //case SDL_EVENT_MOUSE_BUTTON_DOWN:
        //  mouseButtonDown(e.button);
        //  break;
        //case SDL_EVENT_MOUSE_BUTTON_UP:
        //  mouseButtonUp(e.button);
        //  break;
        //case SDL_MOUSEWHEEL:
        //  mouseWheel(e.wheel);
        //  break;

        /* gamepad events */
        //case SDL_CONTROLLERAXISMOTION:
        //  axisMotion(e.caxis);
        //  break;
        //case SDL_CONTROLLERBUTTONDOWN:
        //  buttonDown(e.cbutton);
        //  break;
        //case SDL_CONTROLLERBUTTONUP:
        //  buttonUp(e.cbutton);
        //  break;
        //case SDL_CONTROLLERDEVICEADDED:
        //  gamepadAdded(e.cdevice);
        //  break;
        //case SDL_CONTROLLERDEVICEREMOVED:
        //  gamepadRemoved(e.cdevice);
        //  break;
        //case SDL_CONTROLLERDEVICEREMAPPED:
        //  break;

        //case SDL_CONTROLLERSTEAMHANDLEUPDATED: // TODO: implement when steam is done
        //  break;

        //case SDL_AUDIODEVICEADDED:
        //  break;
        //case SDL_AUDIODEVICEREMOVED:
        //  break;

        //case SDL_POLLSENTINEL:
        //  break;
    }
  }
}
