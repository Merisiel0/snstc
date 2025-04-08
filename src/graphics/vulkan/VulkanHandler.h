
#pragma once

#include "VulkanUtils.h"
#include <vector>

class Instance;
class ImmediateSubmit;
class DebugUtilsMessenger;
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
  std::shared_ptr<Instance> _instance;

#ifdef VKDEBUG
  std::shared_ptr<DebugUtilsMessenger> _debugMessenger;
#endif // VKDEBUG

  std::shared_ptr<PhysicalDevice> _physicalDevice;
  std::shared_ptr<Device> _device;
  std::shared_ptr<Allocator> _allocator;

  std::shared_ptr<ImmediateSubmit> _immediateSubmit;

  public:
  std::shared_ptr<Window> _window;
  
  private:
  std::shared_ptr<Swapchain> _swapchain;
  std::shared_ptr<Image> _drawImage;
  std::shared_ptr<Image> _depthImage;

  std::shared_ptr<GraphicsPipeline> _pipelinePBR;
  std::shared_ptr<GraphicsPipeline> _pipelineLinePBR;
  std::shared_ptr<GraphicsPipeline> _pipelineColor;
  std::shared_ptr<GraphicsPipeline> _pipelineLineColor;

  std::shared_ptr<Sampler> _defaultSampler;

  std::shared_ptr<DescriptorPool> _descriptorPool;
  std::shared_ptr<DescriptorSetLayout> _camDescSetLayout;
  std::shared_ptr<DescriptorSetLayout> _objDescSetLayout;

  void beginDrawing(World& world);
  void endDrawing();

  public:
  VulkanHandler(const char* applicationName, int applicationVersion, const char* engineName, int engineVersion);
  ~VulkanHandler();

  void render(World& world);
  void waitForEndOfWork() const;
};