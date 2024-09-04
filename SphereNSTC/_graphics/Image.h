#pragma once

#include "VulkanUtils.h"

class Device;
class Allocator;
class ImmediateSubmit;
class CommandBuffer;

class Image {
private:
  static inline VkDevice* _devicePtr{ nullptr };
  static inline VmaAllocator* _allocatorPtr{ nullptr };
  static inline ImmediateSubmit* _immediateSubmitPtr{ nullptr };

  VmaAllocation _allocation{ VK_NULL_HANDLE };
  VmaAllocationInfo _info{};
  VkImageLayout _layout{ VK_IMAGE_LAYOUT_UNDEFINED };

  VkImageCreateInfo getCreateInfo(VkImageUsageFlags usage = 0) const;
  VmaAllocationCreateInfo getAllocationInfo() const;
  VkRenderingAttachmentInfo getRenderingAttachmentInfo(VkClearValue* clear) const;

public:
  VkImage handle{ VK_NULL_HANDLE };
  VkImageView view{ VK_NULL_HANDLE };
  VkExtent2D extent{ 0,0 };
  VkFormat format{ VK_FORMAT_UNDEFINED };

  static void init(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit);

  Image() {}
  Image(VkImage image, VkImageView view)
    : handle{ image }, view{ view } {
  }
  Image(VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent,
    VkImageAspectFlags aspectFlags, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);
  Image(const char* path);
  ~Image();

  VkImageLayout layout() const { return _layout; }

  void transitionLayout(CommandBuffer* commandBuffer, VkImageLayout newLayout);
  VkRenderingInfo getRenderingInfo(VkClearValue* clear = nullptr) const;
  void copyTo(CommandBuffer* commandBuffer, Image* image) const;
};