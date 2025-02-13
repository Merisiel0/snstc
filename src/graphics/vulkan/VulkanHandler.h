
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
class DescriptorPool;
class DescriptorSetLayout;
class DescriptorSet;
class Sampler;

class World;

enum WindowScreenMode {
	WINDOWED = 0,
	FULLSCREEN = SDL_WINDOW_FULLSCREEN
};

// Opens a window and renders worlds on it.
class VulkanHandler {
private:
  ImmediateSubmit* _immediateSubmit;

#ifdef VKDEBUG
  DebugUtilsMessenger* _debugMessenger;
#endif // VKDEBUG

  Instance* _instance;
  PhysicalDevice* _physicalDevice;
  Device* _device;
  Allocator* _allocator;
public:
  Window* _window;
private:
  Swapchain* _swapchain;
  Image* _drawImage;
  Image* _depthImage;

  GraphicsPipeline* _pipelinePBR;
  GraphicsPipeline* _pipelineLinePBR;
  GraphicsPipeline* _pipelineColor;
  GraphicsPipeline* _pipelineLineColor;

  Sampler* _defaultSampler;

  DescriptorPool* _descriptorPool;
  DescriptorSetLayout* _camDescSetLayout;
  DescriptorSetLayout* _objDescSetLayout;

  void beginDrawing(World* world);
  void endDrawing();

public:
  VulkanHandler(const char* applicationName, int applicationVersion, const char* engineName, int engineVersion);
  ~VulkanHandler();

  void render(World* world);
  void waitForEndOfWork() const;
};