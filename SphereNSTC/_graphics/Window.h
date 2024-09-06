#pragma once

#include "VulkanUtils.h"

class Window {
public:
  SDL_Window* handle{ nullptr };
  glm::uvec2 position{};
  glm::uvec2 extent{};

  Window() {}
  Window(const char* title);
  ~Window();
};