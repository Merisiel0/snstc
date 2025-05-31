#pragma once

#include "Frame.h"
#include "Image.h"
#include "VulkanUtils.h"

#include <array>
#include <vector>

class Window;
class Instance;
class CommandBuffer;
class DescriptorPool;
class DescriptorSetLayout;

class Swapchain {
private:
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
  int getFrameIndex() const { return frameNumber % FRAME_OVERLAP; }

  Swapchain(const Window& window);

  ~Swapchain();

  uint32_t acquireNextImage() const;
  std::shared_ptr<Image> getImage(uint32_t index) const;
};