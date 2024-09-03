#include "Window.h"

Window::Window(const char* title) {
  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  handle = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    dm.w,
    dm.h,
    SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);
}

Window::~Window() {
  SDL_DestroyWindow(handle);
}