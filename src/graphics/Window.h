#pragma once

#include "vulkan/VulkanUtils.h"

class Instance;

class Window {
  private:
  Instance* _instancePtr;
  SDL_Surface* _iconSurface;

  public:
  SDL_Window* handle;
  glm::vec2 position;
  glm::uvec2 extent;
  VkSurfaceKHR surface;

  Window(const char* title, Instance* instance);
  ~Window();

  void setIcon(const char* path);
};