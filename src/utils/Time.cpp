#include "Time.h"

/**
 * @brief Initializes time for deltas.
 * 
 */
void Time::init() {
  now = SDL_GetTicksNS();
  deltaTime = 0;
}

/**
 * @brief Updates program delta time.
 * 
 */
void Time::update() {
  last = now;
  now = SDL_GetTicksNS();
  deltaTime = (float) ((now - last) / 1000000000);
}
