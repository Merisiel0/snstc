#pragma once

#include "VulkanUtils.h"

class Window;
class Device;
class Allocator;
class ImmediateSubmit;
class CommandBuffer;
class Buffer;
class Swapchain;
class ResourceManager;

enum ImageType{
  COLOR, DEPTH
};

class Image {
private:
  static inline std::weak_ptr<Device> _device;
  static inline std::weak_ptr<Allocator> _allocator;
  static inline std::weak_ptr<ImmediateSubmit> _immediateSubmit;

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
  VkRenderingAttachmentInfo getRenderingAttachmentInfo(const VkClearValue& clear = VkClearValue{}, bool doClear = false) const;

  VkImageSubresourceRange getSubresourceRange() const;

  Image(const char* path);
  
  friend class ResourceManager;

public:
  static inline VkColorSpaceKHR colorSpace;
  static inline VkFormat colorFormat;
  static inline VkImageAspectFlags colorAspect;
  static inline VkFormat depthFormat;
  static inline VkImageAspectFlags depthAspect;

  VkImage handle;
  VkImageView view;

  VkImageSubresourceLayers getSubresourceLayers() const;

  static void init(std::shared_ptr<Device> device, std::weak_ptr<Allocator> allocator, std::weak_ptr<ImmediateSubmit> immediateSubmit);

  Image(VkImage image, VkImageView view, VkExtent2D extent);
  Image(const Swapchain& swapchain, ImageType type, VkImageUsageFlags usageFlags);
  ~Image();

  VkImageLayout layout() const { return _layout; }
  VkExtent2D extent() const { return _extent; }
  VkFormat format() const { return _format; }

  static VkRenderingInfo getRenderingInfo(const Image& color, const Image& depth, const VkClearValue& clear = VkClearValue{}, bool doClear = false);

  void transitionLayout(CommandBuffer* commandBuffer, VkImageLayout newLayout);
  void transitionFormat(CommandBuffer* commandBuffer, VkFormat newFormat);
  void blitTo(CommandBuffer* commandBuffer, Image* image) const;
  void copyTo(CommandBuffer* commandBuffer, Image* image) const;
  void copyToBuffer(CommandBuffer* commandBuffer, Buffer* buffer) const;
  void save(const char* path);
  void clear(CommandBuffer* commandBuffer, Color color);
};