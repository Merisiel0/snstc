#pragma once

#include "VulkanUtils.h"

class Window;
class Device;
class Allocator;
class ImmediateSubmit;
class CommandBuffer;
class Buffer;
class Swapchain;

class Image {
private:
  static inline Device* _devicePtr;
  static inline VmaAllocator* _allocatorPtr;
  static inline ImmediateSubmit* _immediateSubmitPtr;

  VmaAllocation _allocation;
  VmaAllocationInfo _info;
  VkImageLayout _layout{ VK_IMAGE_LAYOUT_UNDEFINED };
  int _channelAmount;
  VkExtent2D _extent;
  VkFormat _format;
  VkImageAspectFlags _aspect;
  VkImageUsageFlags _usage;
    
  VkImageCreateInfo getCreateInfo() const;
  VkImageViewCreateInfo getViewCreateInfo() const;
  VmaAllocationCreateInfo getAllocationInfo() const;
  VkRenderingAttachmentInfo getRenderingAttachmentInfo(VkClearValue* clear) const;

  VkImageSubresourceRange getSubresourceRange() const;

public:
  static inline VkColorSpaceKHR colorSpace;
  static inline VkFormat colorFormat;
  static inline VkFormat depthFormat;

  VkImage handle;
  VkImageView view;

  VkImageSubresourceLayers getSubresourceLayers() const;

  static void init(Window* window, Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit);

  Image(VkImage image, VkImageView view, VkExtent2D extent);
  Image(Swapchain* swapchain, VkImageAspectFlags aspectFlags, VkImageUsageFlags usageFlags);
  Image(const char* path);
  ~Image();

  VkImageLayout layout() const { return _layout; }
  VkExtent2D extent() const { return _extent; }
  VkFormat format() const { return _format; }

  static VkRenderingInfo getRenderingInfo(Image* color, Image* depth, VkClearValue* clear = nullptr);

  void transitionLayout(CommandBuffer* commandBuffer, VkImageLayout newLayout);
  void transitionFormat(CommandBuffer* commandBuffer, VkFormat newFormat);
  void blitTo(CommandBuffer* commandBuffer, Image* image) const;
  void copyTo(CommandBuffer* commandBuffer, Image* image) const;
  void copyToBuffer(CommandBuffer* commandBuffer, Buffer* buffer) const;
  void save(const char* path);
  void clear(CommandBuffer* commandBuffer, Color color);
};