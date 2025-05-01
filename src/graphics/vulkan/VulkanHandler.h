
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

enum WindowScreenMode { WINDOWED = 0, FULLSCREEN = SDL_WINDOW_FULLSCREEN };

/// @brief List containing all implemented graphics pipelines.
enum GraphicsPipelineId {
  /// @brief Reserved for skyboxes.
  GRAPHICS_PIPELINE_SKYBOX,

  // UNLIT_POINT_LIST,
  // UNLIT_LINE_LIST,
  // UNLIT_LINE_STRIP,

  // UNLIT_POINT_TRIANGLE_LIST,
  // UNLIT_LINE_TRIANGLE_LIST,
  UNLIT_FILL_TRIANGLE_LIST,

  // UNLIT_POINT_TRIANGLE_STRIP,
  // UNLIT_LINE_TRIANGLE_STRIP,
  // UNLIT_FILL_TRIANGLE_STRIP,

  // UNLIT_POINT_TRIANGLE_FAN,
  // UNLIT_LINE_TRIANGLE_FAN,
  // UNLIT_FILL_TRIANGLE_FAN,

  // UNLIT_INSTANCE_POINT_LIST,
  // UNLIT_INSTANCE_LINE_LIST,
  // UNLIT_INSTANCE_LINE_STRIP,

  // UNLIT_INSTANCE_POINT_TRIANGLE_LIST,
  // UNLIT_INSTANCE_LINE_TRIANGLE_LIST,
  // UNLIT_INSTANCE_FILL_TRIANGLE_LIST,

  // UNLIT_INSTANCE_POINT_TRIANGLE_STRIP,
  // UNLIT_INSTANCE_LINE_TRIANGLE_STRIP,
  // UNLIT_INSTANCE_FILL_TRIANGLE_STRIP,

  // UNLIT_INSTANCE_POINT_TRIANGLE_FAN,
  // UNLIT_INSTANCE_LINE_TRIANGLE_FAN,
  // UNLIT_INSTANCE_FILL_TRIANGLE_FAN,

  // UNLIT_PARTICLE_POINT_LIST,
  // UNLIT_PARTICLE_LINE_LIST,
  // UNLIT_PARTICLE_LINE_STRIP,

  // UNLIT_PARTICLE_POINT_TRIANGLE_LIST,
  // UNLIT_PARTICLE_LINE_TRIANGLE_LIST,
  // UNLIT_PARTICLE_FILL_TRIANGLE_LIST,

  // UNLIT_PARTICLE_POINT_TRIANGLE_STRIP,
  // UNLIT_PARTICLE_LINE_TRIANGLE_STRIP,
  // UNLIT_PARTICLE_FILL_TRIANGLE_STRIP,

  // UNLIT_PARTICLE_POINT_TRIANGLE_FAN,
  // UNLIT_PARTICLE_LINE_TRIANGLE_FAN,
  // UNLIT_PARTICLE_FILL_TRIANGLE_FAN,

  GRAPHICS_PIPELINE_ID_COUNT
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

  std::vector<std::shared_ptr<GraphicsPipeline>> _graphicsPipelines;

  std::shared_ptr<Sampler> _defaultSampler;

  std::shared_ptr<DescriptorPool> _descriptorPool;
  std::shared_ptr<DescriptorSetLayout> _globalDescSetLayout;
  std::shared_ptr<DescriptorSetLayout> _skyboxDescSetLayout;
  std::shared_ptr<DescriptorSetLayout> _materialDescSetLayout;

  void beginDrawing(World& world);
  void endDrawing();

public:
  VulkanHandler(const char* applicationName, int applicationVersion, const char* engineName,
    int engineVersion);
  ~VulkanHandler();

  void render(World& world);
  void waitForEndOfWork() const;
};