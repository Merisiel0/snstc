#pragma once

#include "Utils.h"

class Time {
  static inline Uint64 now;
  static inline Uint64 last;

public:
  // deltaTime in seconds
  static inline float deltaTime;

  static void init();
  static void update();
};