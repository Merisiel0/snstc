#pragma once

#include "VulkanUtils.h"

class Frame;
class Window;
class Instance;
class PhysicalDevice;
class Device;
class Image;
class CommandBuffer;
class DescriptorPool;
class DescriptorSetLayout;

class Swapchain {
private:
  VkDevice* _devicePtr;

  Image** _images;
  uint32_t* _imageCount;

  VkSwapchainCreateInfoKHR getCreateInfo(VkSurfaceKHR suface, uint32_t imageCount,
    VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode) const;

public:
  static const unsigned int FRAME_OVERLAP = 2;

  VkSwapchainKHR handle;
  VkExtent2D extent;

  uint32_t frameNumber;
  Frame** frames;
  Frame* getCurrentFrame() const { return frames[frameNumber % FRAME_OVERLAP]; }

  Swapchain(Window* window, Device* device, DescriptorPool* descriptorPool,
    DescriptorSetLayout* vertLayout, DescriptorSetLayout* fragLayout);
  ~Swapchain();

  uint32_t acquireNextImage() const;
  Image* getImage(uint32_t index) const;
};