#pragma once

#include "VulkanUtils.h"
#include "resources/Resource.h"

class Window;
class Device;
class Allocator;
class ImmediateSubmit;
class CommandBuffer;
class Buffer;
class Swapchain;
class ResourceManager;

enum ImageType { COLOR, DEPTH };

class Image : Resource<Image, std::string> {
private:
  static const std::string SWAPCHAIN_IMAGE_TAG;

  static inline std::weak_ptr<Device> _device;
  static inline std::weak_ptr<Allocator> _allocator;
  static inline std::weak_ptr<ImmediateSubmit> _immediateSubmit;

  VmaAllocation _allocation;
  VmaAllocationInfo _info;
  VkImageLayout _layout {VK_IMAGE_LAYOUT_UNDEFINED};
  int _channelAmount;
  VkExtent2D _extent;
  VkFormat _format;
  VkImageAspectFlags _aspect;
  VkImageUsageFlags _usage;
  uint _layerCount;

  VkImageCreateInfo getCreateInfo(VkImageCreateFlagBits flags = (VkImageCreateFlagBits) 0) const;
  VkImageViewCreateInfo getViewCreateInfo() const;
  VmaAllocationCreateInfo getAllocationInfo() const;
  VkRenderingAttachmentInfo getRenderingAttachmentInfo(const VkClearValue& clear = VkClearValue {},
    bool doClear = false) const;

  VkImageSubresourceRange getSubresourceRange() const;

  Image(std::string path);
  Image(std::vector<std::string> faces);
  Image(uint32_t width, uint32_t height, Color color);

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

  static void init(std::shared_ptr<Device> device, std::weak_ptr<Allocator> allocator,
    std::weak_ptr<ImmediateSubmit> immediateSubmit);

  static std::shared_ptr<Image> load(std::string path);
  static std::shared_ptr<Image> load(std::vector<std::string> faces);
  static std::shared_ptr<Image> load(uint32_t width, uint32_t height, Color color);

  Image(VkImage image, VkImageView view, VkExtent2D extent);
  Image(const Swapchain& swapchain, ImageType type, VkImageUsageFlags usageFlags);
  ~Image();

  VkImageLayout layout() const { return _layout; }
  VkExtent2D extent() const { return _extent; }
  VkFormat format() const { return _format; }

  static RenderingInfoData getRenderingInfo(const Image& color, const Image& depth,
    const VkClearValue& clear = VkClearValue {}, bool doClear = false);

  void transitionLayout(std::shared_ptr<CommandBuffer> commandBuffer, VkImageLayout newLayout);
  // void transitionFormat(std::shared_ptr<CommandBuffer> commandBuffer, VkFormat newFormat);
  void blitTo(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<Image> image) const;
  void copyTo(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<Image> image) const;
  void copyToBuffer(std::shared_ptr<CommandBuffer> commandBuffer,
    std::shared_ptr<Buffer> buffer) const;
  void save(const char* path);
  void clear(std::shared_ptr<CommandBuffer> commandBuffer, Color color);
};