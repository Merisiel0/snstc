#pragma once

#include "VulkanUtils.h"

class Frame;
class Window;
class Instance;
class PhysicalDevice;
class Device;
class Image;
class CommandBuffer;

class Swapchain {
private:
  VkInstance* _instancePtr{ nullptr };
  VkDevice* _devicePtr{ nullptr };

  VkSurfaceKHR _surface{ VK_NULL_HANDLE };
  Image** _images{ nullptr };
  uint32_t* _imageCount{ nullptr };

  static const unsigned int FRAME_OVERLAP = 2;

public:
  VkSwapchainKHR handle{ VK_NULL_HANDLE };
  VkFormat imageFormat{ VK_FORMAT_UNDEFINED };
  VkExtent2D extent{ 0,0 };

  uint32_t frameNumber{ 0 };
  Frame** frames{ nullptr };
  Frame* getCurrentFrame() const { return frames[frameNumber % FRAME_OVERLAP]; }

  Swapchain() {}
  Swapchain(Window* window, Instance* instance, PhysicalDevice* physicalDevice, Device* device);
  ~Swapchain();

  uint32_t acquireNextImage() const;
  Image* getImage(uint32_t index) const;
};