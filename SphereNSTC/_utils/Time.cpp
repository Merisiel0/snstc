#include "Time.h"

void Time::init() {
  now = SDL_GetTicks();
  deltaTime = 0;
}

void Time::update() {
  last = now;
  now = SDL_GetTicks();
  deltaTime = (float)(now - last);
  deltaTime /= 1000;
}
