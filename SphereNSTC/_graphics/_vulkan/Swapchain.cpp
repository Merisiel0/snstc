#include "Swapchain.h"

#include "_graphics/Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Frame.h"
#include "Image.h"
#include "Semaphore.h"

VkSwapchainCreateInfoKHR Swapchain::getCreateInfo(VkSurfaceKHR surface, uint32_t imageCount,
  VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode) const {
  VkSwapchainCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  //info.pNext = nullptr;
  //info.flags = 0;
  info.surface = surface;
  info.minImageCount = imageCount;
  info.imageFormat = Image::colorFormat;
  info.imageColorSpace = Image::colorSpace;
  info.imageExtent = capabilities.currentExtent;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //info.queueFamilyIndexCount = 0;
  //info.pQueueFamilyIndices = nullptr;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = presentMode;
  info.clipped = VK_TRUE;
  //info.oldSwapchain = nullptr;

  return info;
}

Swapchain::Swapchain(Window* window, Device* device, DescriptorPool* descriptorPool,
  DescriptorSetLayout* vertLayout, DescriptorSetLayout* fragLayout) {
  this->_devicePtr = &device->handle;

  frames = new Frame * [FRAME_OVERLAP];
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    frames[i] = new Frame(device, descriptorPool, vertLayout, fragLayout);
  }

  extent.width = window->extent.x;
  extent.height = window->extent.y;

  uint32_t presentModeCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice()->handle, window->surface, &presentModeCount, nullptr));
  std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice()->handle, window->surface, &presentModeCount, availablePresentModes.data()));

  const std::vector<VkPresentModeKHR> preferredPresentModes = {
      VK_PRESENT_MODE_MAILBOX_KHR,
      VK_PRESENT_MODE_FIFO_KHR,
      VK_PRESENT_MODE_IMMEDIATE_KHR
  };
  VkPresentModeKHR presentMode{};

  for (const auto& preferredPresentMode : preferredPresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (preferredPresentMode == availablePresentMode) {
        presentMode = preferredPresentMode;
        goto presentModeChosen;
      }
    }
  }
  throw std::runtime_error("No preferred present mode available.");
presentModeChosen:


  VkSurfaceCapabilitiesKHR capabilities{};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice()->handle, window->surface, &capabilities));

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = getCreateInfo(window->surface, imageCount, capabilities, presentMode);
  VK_CHECK(vkCreateSwapchainKHR(device->handle, &createInfo, nullptr, &handle));

  // get swapchain images
  _imageCount = new uint32_t;
  VK_CHECK(vkGetSwapchainImagesKHR(device->handle, handle, _imageCount, nullptr));
  std::vector<VkImage> vkImages(*_imageCount);
  VK_CHECK(vkGetSwapchainImagesKHR(device->handle, handle, _imageCount, vkImages.data()));

  _images = new Image * [*_imageCount];
  // get swapchain image views
  std::vector<VkImageView> imageViews(*_imageCount);
  for (uint32_t i = 0; i < *_imageCount; i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //createInfo.pNext = nullptr;
    //createInfo.flags = 0;
    createInfo.image = vkImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = Image::colorFormat;
    //createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    //createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    //createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    //createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    //createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device->handle, &createInfo, nullptr, &imageViews[i]));

    _images[i] = new Image(vkImages[i], imageViews[i], extent);
  }
}

Swapchain::~Swapchain() {
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    delete frames[i];
  }
  delete[] frames;

  for (uint32_t i = 0; i < *_imageCount; i++) {
    vkDestroyImageView(*_devicePtr, _images[i]->view, nullptr);
  }
  delete _imageCount;

  vkDestroySwapchainKHR(*_devicePtr, handle, nullptr);
}

uint32_t Swapchain::acquireNextImage() const {
  uint32_t swapchainImageIndex;
  VK_CHECK(vkAcquireNextImageKHR(*_devicePtr, handle, 9999999999, getCurrentFrame()->swapchainSemaphore->handle, nullptr, &swapchainImageIndex));
  return swapchainImageIndex;
}

Image* Swapchain::getImage(uint32_t index) const {
  return _images[index];
}
