#pragma once

#include "VulkanUtils.h"

class Device;
class Allocator;
class CommandBuffer;

class Image {
private:
  static VkDevice _devicePtr;
  static VmaAllocator _allocatorPtr;

  VmaAllocation _allocation{ VK_NULL_HANDLE };
  VkImageLayout _layout{ VK_IMAGE_LAYOUT_UNDEFINED };

  VkImageCreateInfo getCreateInfo(VkImageUsageFlags usage);
  VkRenderingAttachmentInfo getRenderingAttachmentInfo(VkClearValue* clear) const;

public:
  VkImage handle{ VK_NULL_HANDLE };
  VkImageView view{ VK_NULL_HANDLE };
  VkExtent2D extent{ 0,0 };
  VkFormat format{ VK_FORMAT_UNDEFINED };

  static void init(Device* device, Allocator* allocator);

  Image() {}
  Image(VkImage image, VkImageView view)
    : handle{ image }, view{ view } {
  }
  Image(VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent,
    VkImageAspectFlags aspectFlags, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);
  Image(const char* path);
  ~Image();

  void transitionLayout(CommandBuffer* commandBuffer, VkImageLayout newLayout) const;
  VkRenderingInfo getRenderingInfo(VkClearValue* clear = nullptr) const;
  void copyTo(CommandBuffer* commandBuffer, Image* image) const;
};