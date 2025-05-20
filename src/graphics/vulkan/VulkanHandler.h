
#pragma once

#include "VulkanUtils.h"

#include <memory>
#include <unordered_map>
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
class GraphicsPipelineId;

class World;

// Opens a window and renders worlds on it.
class VulkanHandler {
private:
  static std::shared_ptr<Instance> _instance;

#ifdef VKDEBUG
  static std::shared_ptr<DebugUtilsMessenger> _debugMessenger;
#endif // VKDEBUG

  static std::shared_ptr<PhysicalDevice> _physicalDevice;
  static std::shared_ptr<Device> _device;
  static std::shared_ptr<Allocator> _allocator;

  static std::shared_ptr<ImmediateSubmit> _immediateSubmit;

public:
  static std::shared_ptr<Window> _window;

private:
  static std::shared_ptr<Swapchain> _swapchain;
  static std::shared_ptr<Image> _drawImage;
  static std::shared_ptr<Image> _depthImage;

  static std::unordered_map<GraphicsPipelineId, std::shared_ptr<GraphicsPipeline>> _graphicsPipelines;

  static std::shared_ptr<Sampler> _defaultSampler;

  static void beginDrawing(World& world);
  static void endDrawing();

public:
  static void init(const char* applicationName, int applicationVersion, const char* engineName,
    int engineVersion);
  static void cleanup();

  static void loadPipeline(GraphicsPipelineId id);
  static void releasePipeline(GraphicsPipelineId id);

  static void render(World& world);
  static void waitForEndOfWork();
};