#include "Time.h"

/**
 * @brief Initializes time for deltas.
 * 
 */
void Time::init() {
  now = SDL_GetTicks();
  deltaTime = 0;
}

/**
 * @brief Updates program delta time.
 * 
 */
void Time::update() {
  last = now;
  now = SDL_GetTicks();
  deltaTime = (float) (now - last);
  deltaTime /= 1000;
}
