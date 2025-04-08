#pragma once

#include "Frame.h"
#include "Image.h"
#include "VulkanUtils.h"

#include <array>

class Window;
class Instance;
class PhysicalDevice;
class Device;
class CommandBuffer;
class DescriptorPool;
class DescriptorSetLayout;

class Swapchain {
private:
  std::shared_ptr<Device> _device;

  std::vector<std::shared_ptr<Image>> _images;

  VkSurfaceFormatKHR _surfaceFormat;

  VkSwapchainCreateInfoKHR getCreateInfo(VkSurfaceKHR suface, uint32_t imageCount,
    VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode) const;

public:
  static const unsigned int FRAME_OVERLAP = 2;

  VkSwapchainKHR handle;
  VkExtent2D extent;

  uint32_t frameNumber;
  std::array<std::shared_ptr<Frame>, FRAME_OVERLAP> frames;
  std::shared_ptr<Frame> getCurrentFrame() const { return frames[frameNumber % FRAME_OVERLAP]; }

  Swapchain(const Window& window, std::shared_ptr<Device> device,
    const DescriptorPool& descriptorPool, const DescriptorSetLayout& vertLayout,
    const DescriptorSetLayout& fragLayout);

  ~Swapchain();

  uint32_t acquireNextImage() const;
  std::shared_ptr<Image> getImage(uint32_t index) const;
};