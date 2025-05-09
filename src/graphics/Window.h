#pragma once

#include "vulkan/VulkanUtils.h"

class Instance;

class Window {
private:
  std::shared_ptr<Instance> _instance;
  SDL_Surface* _iconSurface;

public:
  SDL_Window* handle;
  glm::vec2 position;
  glm::uvec2 extent;
  VkSurfaceKHR surface;

  Window(std::string title, std::shared_ptr<Instance> instance);
  ~Window();

  void setIcon(std::string path);
};