#include "Window.h"

#include "stb_image.h"
#include "vulkan/Instance.h"

Window::Window(const char* title, std::shared_ptr<Instance> instance) {
  _instance = instance;
  _iconSurface = nullptr;

  SDL_DisplayID* displays = SDL_GetDisplays(nullptr);
  const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(displays[0]);
  extent = {dm->w, dm->h};
  //extent = { dm->w * .8f, dm->h * .8f };

  handle =
    SDL_CreateWindow(title, extent.x, extent.y, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  if(!handle) { std::clog << SDL_GetError(); }

  int x, y;
  SDL_GetWindowPosition(handle, &x, &y);
  position = {x, y};

  SDL_free(displays);

  SDL_CHECK(SDL_Vulkan_CreateSurface(handle, instance->handle, nullptr, &surface));
}

Window::~Window() {
  vkDestroySurfaceKHR(_instance->handle, surface, nullptr);

  SDL_DestroyWindow(handle);

  SDL_DestroySurface(_iconSurface);
}

void Window::setIcon(const char* path) {
  int width, height, channels;
  unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

  if(!data) {
    std::cerr << "Failed to load image: " << path << std::endl;
    return;
  }

  SDL_DestroySurface(_iconSurface);

  _iconSurface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888, data, 0);

  SDL_SetWindowIcon(handle, _iconSurface);

  stbi_image_free(data);
}
