#include "Swapchain.h"

#include "Device.h"
#include "Frame.h"
#include "Image.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Semaphore.h"
#include "graphics/Window.h"

VkSwapchainCreateInfoKHR Swapchain::getCreateInfo(VkSurfaceKHR surface, uint32_t imageCount,
  VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode) const {
  VkSwapchainCreateInfoKHR info {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  // info.pNext = nullptr;
  // info.flags = 0;
  info.surface = surface;
  info.minImageCount = imageCount;
  info.imageFormat = _surfaceFormat.format;
  info.imageColorSpace = _surfaceFormat.colorSpace;
  info.imageExtent = capabilities.currentExtent;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  // info.queueFamilyIndexCount = 0;
  // info.pQueueFamilyIndices = nullptr;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = presentMode;
  info.clipped = VK_TRUE;
  // info.oldSwapchain = nullptr;

  return info;
}

Swapchain::Swapchain(const Window& window, std::shared_ptr<Device> device,
  const DescriptorPool& descriptorPool, const DescriptorSetLayout& vertLayout,
  const DescriptorSetLayout& fragLayout) {
  _device = device;

  for(int i = 0; i < FRAME_OVERLAP; i++) {
    frames[i] = std::make_shared<Frame>(device, descriptorPool, vertLayout, fragLayout);
  }

  extent.width = window.extent.x;
  extent.height = window.extent.y;

  // find surface format
  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice()->handle, window.surface,
    &surfaceFormatCount, nullptr);
  std::vector<VkSurfaceFormatKHR> availableSurfaceFormats(surfaceFormatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice()->handle, window.surface,
    &surfaceFormatCount, availableSurfaceFormats.data());

  const std::vector<VkFormat> preferredFormats = {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UINT,
    VK_FORMAT_B8G8R8A8_SRGB};

  const std::vector<VkColorSpaceKHR> preferredColorSpaces = {VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

  Swapchain::_surfaceFormat.format = VK_FORMAT_UNDEFINED;
  for(const auto& colorSpace : preferredColorSpaces) {
    for(const auto& format : preferredFormats) {
      for(const auto& availableSurfaceFormat : availableSurfaceFormats) {
        if(availableSurfaceFormat.format == format &&
           availableSurfaceFormat.colorSpace == colorSpace) {
          Swapchain::_surfaceFormat = availableSurfaceFormat;
          break;
        }
      }
    }
  }
  if(Swapchain::_surfaceFormat.format == VK_FORMAT_UNDEFINED) {
    throw std::runtime_error("No prefered surface format available.");
  }

  // find present mode
  uint32_t presentModeCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice()->handle,
    window.surface, &presentModeCount, nullptr));
  std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice()->handle,
    window.surface, &presentModeCount, availablePresentModes.data()));

  const std::vector<VkPresentModeKHR> preferredPresentModes = {VK_PRESENT_MODE_MAILBOX_KHR,
    VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR};
  VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;

  for(const auto& preferredPresentMode : preferredPresentModes) {
    for(const auto& availablePresentMode : availablePresentModes) {
      if(preferredPresentMode == availablePresentMode) {
        presentMode = preferredPresentMode;
        break;
      }
    }
  }
  if(presentMode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    throw std::runtime_error("No preferred present mode available.");
  }

  VkSurfaceCapabilitiesKHR capabilities {};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice()->handle,
    window.surface, &capabilities));

  uint32_t imageCount = capabilities.minImageCount + 1;
  if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo =
    getCreateInfo(window.surface, imageCount, capabilities, presentMode);
  VK_CHECK(vkCreateSwapchainKHR(device->handle, &createInfo, nullptr, &handle));

  // get swapchain images
  uint32_t _imageCount = 0;
  VK_CHECK(vkGetSwapchainImagesKHR(device->handle, handle, &_imageCount, nullptr));
  std::vector<VkImage> vkImages(_imageCount);
  VK_CHECK(vkGetSwapchainImagesKHR(device->handle, handle, &_imageCount, vkImages.data()));

  _images.resize(_imageCount);
  // get swapchain image views
  std::vector<VkImageView> imageViews(_imageCount);
  for(uint32_t i = 0; i < _imageCount; i++) {
    VkImageViewCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    // createInfo.pNext = nullptr;
    // createInfo.flags = 0;
    createInfo.image = vkImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = _surfaceFormat.format;
    // createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    // createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    // createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    // createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    // createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device->handle, &createInfo, nullptr, &imageViews[i]));

    _images[i] =
      std::make_shared<Image>(vkImages[i], imageViews[i], extent);
  }
}

Swapchain::~Swapchain() {
  for(size_t i = 0; i < _images.size(); i++) {
    vkDestroyImageView(_device->handle, _images[i]->view, nullptr);
  }

  vkDestroySwapchainKHR(_device->handle, handle, nullptr);
}

uint32_t Swapchain::acquireNextImage() const {
  uint32_t swapchainImageIndex;
  VkSemaphore s = getCurrentFrame()->swapchainSemaphore->handle;
  VK_CHECK(
    vkAcquireNextImageKHR(_device->handle, handle, 9999999999, s, nullptr, &swapchainImageIndex));
  return swapchainImageIndex;
}

std::shared_ptr<Image> Swapchain::getImage(uint32_t index) const { return _images[index]; }
