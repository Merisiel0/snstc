#include "Window.h"

#include "vulkan/Instance.h"

Window::Window(const char* title, Instance* instance) {
  _instancePtr = instance;

  SDL_DisplayID* displays = SDL_GetDisplays(nullptr);
  const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(displays[0]);
  extent = { dm->w, dm->h };
  //extent = { dm->w * .8f, dm->h * .8f };

  handle = SDL_CreateWindow(
    title,
    extent.x,
    extent.y,
    SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  if (!handle) {
    std::clog << SDL_GetError();
  }

  int x, y;
  SDL_GetWindowPosition(handle, &x, &y);
  position = { x, y };

  //delete dm;
  SDL_free(displays);

  SDL_CHECK(SDL_Vulkan_CreateSurface(handle, instance->handle, nullptr, &surface));
}

Window::~Window() {
  vkDestroySurfaceKHR(_instancePtr->handle, surface, nullptr);
  SDL_DestroyWindow(handle);
}