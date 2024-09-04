#pragma once

#include "VulkanUtils.h"
#include <vector>

class ImmediateSubmit;
class DebugUtilsMessenger;
class Instance;
class PhysicalDevice;
class Device;
class Allocator;
class Window;
class Swapchain;
class Image;
class GraphicsPipeline;

class World;

struct GraphicsInitInfo {
	const char* applicationName{ nullptr };
	int applicationVersion{ 0 };
	const char* engineName{ nullptr };
	int engineVersion{ 0 };
};

enum WindowScreenMode {
	WINDOWED = 0,
	FULLSCREEN = SDL_WINDOW_FULLSCREEN,
	FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP
};

// Opens a window and renders Worlds on it.
class GraphicsHandler {
private:
  ImmediateSubmit* _immediateSubmit;

#ifdef VKDEBUG
  DebugUtilsMessenger* _debugMessenger;
#endif // VKDEBUG

  Instance* _instance;
  PhysicalDevice* _physicalDevice;
  Device* _device;
  Allocator* _allocator;

  Window* _window;
  Swapchain* _swapchain;
  Image* _drawImage;

  /* BLAS* triangleBLAS;
   TLAS* worldTLAS;*/
   //Pipeline rayTracingPipeline;

  void beginDrawing();
  void endDrawing();

public:
  GraphicsHandler(GraphicsInitInfo initInfo);
  ~GraphicsHandler();

  GraphicsPipeline* defaultPipeline{ nullptr };

  void Render(World* world);
};