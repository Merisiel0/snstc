#pragma once

#include "VulkanUtils.h"

class Window {
public:
  SDL_Window* handle{ nullptr };

  Window() {}
  Window(const char* title);
  ~Window();
};