#include "Swapchain.h"

#include "Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Frame.h"
#include "Image.h"
#include "Semaphore.h"

Swapchain::Swapchain(Window* window, Instance* instance, PhysicalDevice* physicalDevice, Device* device) {
  this->_instancePtr = &instance->handle;
  this->_devicePtr = &device->handle;

  frames = new Frame * [FRAME_OVERLAP];
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    frames[i] = new Frame(device);
  }

  SDL_BCHECK(SDL_Vulkan_CreateSurface(window->handle, instance->handle, &_surface));

  VkSurfaceCapabilitiesKHR capabilities{};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->handle, _surface, &capabilities));

  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle, _surface, &surfaceFormatCount, nullptr);
  std::vector<VkSurfaceFormatKHR> availableSurfaceFormats(surfaceFormatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle, _surface, &surfaceFormatCount, availableSurfaceFormats.data());

  const std::vector<VkFormat> preferredFormats = {
      VK_FORMAT_R32G32B32A32_UINT,
      VK_FORMAT_R16G16B16A16_UNORM,
      VK_FORMAT_R16G16B16A16_UINT,
      VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_R8G8B8A8_UINT
  };

  const std::vector<VkColorSpaceKHR> preferredColorSpaces = {
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    VK_COLOR_SPACE_HDR10_HLG_EXT,
    VK_COLOR_SPACE_HDR10_ST2084_EXT
  };

  VkSurfaceFormatKHR surfaceFormat{};
  for (const auto& colorSpace : preferredColorSpaces) {
    for (const auto& format : preferredFormats) {
      for (const auto& availableSurfaceFormat : availableSurfaceFormats) {
        if (availableSurfaceFormat.format == format && availableSurfaceFormat.colorSpace == colorSpace) {
          surfaceFormat = availableSurfaceFormat;
          goto surfaceFormatChosen;
        }
      }
    }
  }
surfaceFormatChosen:

  uint32_t presentModeCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle, _surface, &presentModeCount, nullptr));
  std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle, _surface, &presentModeCount, availablePresentModes.data()));

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
presentModeChosen:

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  //createInfo.pNext = nullptr;
  //createInfo.flags = 0;
  createInfo.surface = _surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent = capabilities.currentExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //createInfo.queueFamilyIndexCount = 0;
  //createInfo.pQueueFamilyIndices = nullptr;
  createInfo.preTransform = capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  //createInfo.oldSwapchain = nullptr;

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
    createInfo.format = imageFormat;
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

    _images[i] = new Image(device->handle, vkImages[i], imageViews[i]);
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
  vkDestroySurfaceKHR(*_instancePtr, _surface, nullptr);
}

uint32_t Swapchain::acquireNextImage() const {
  uint32_t swapchainImageIndex;
  VK_CHECK(vkAcquireNextImageKHR(*_devicePtr, handle, 9999999999, getCurrentFrame()->swapchainSemaphore->handle, nullptr, &swapchainImageIndex));
  return swapchainImageIndex;
}

Image* Swapchain::getImage(uint32_t index) const {
  return _images[index];
}
