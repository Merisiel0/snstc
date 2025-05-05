
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

class World;

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

  std::unordered_map<GraphicsPipelineId, std::shared_ptr<GraphicsPipeline>> _graphicsPipelines;

  std::shared_ptr<Sampler> _defaultSampler;

  void beginDrawing(World& world);
  void endDrawing();

public:
  VulkanHandler(const char* applicationName, int applicationVersion, const char* engineName,
    int engineVersion);
  ~VulkanHandler();

  void render(World& world);
  void waitForEndOfWork() const;
};