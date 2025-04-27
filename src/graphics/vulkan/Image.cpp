#include "Image.h"

#include "Allocator.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "Swapchain.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define __STDC_LIB_EXT1__
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"

const std::string Image::SWAPCHAIN_IMAGE_TAG {"swapchain_image"};

void Image::init(std::shared_ptr<Device> device, std::weak_ptr<Allocator> allocator,
  std::weak_ptr<ImmediateSubmit> immediateSubmit) {
  _device = device;
  _allocator = allocator;
  _immediateSubmit = immediateSubmit;

  // find color format
  const std::vector<VkFormat> preferredColorFormats = {
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8G8B8A8_USCALED,
    VK_FORMAT_R8G8B8A8_SSCALED,
  };

  Image::colorFormat = VK_FORMAT_UNDEFINED;
  for(const auto& format : preferredColorFormats) {
    VkImageFormatProperties properties;
    VkResult result = vkGetPhysicalDeviceImageFormatProperties(device->physicalDevice()->handle,
      format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      0, &properties);
    if(result == VK_SUCCESS) {
      Image::colorFormat = format;
      Image::colorAspect = VK_IMAGE_ASPECT_COLOR_BIT;
      break;
    }

    // vkGetPhysicalDeviceFormatProperties(device->physicalDevice()->handle,
    // format, &properties); if (properties.optimalTilingFeatures &
    // VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    //   Image::colorFormat = format;
    //   break;
    // }
  }
  if(Image::colorFormat == VK_FORMAT_UNDEFINED) {
    throw std::runtime_error("No prefered color image format available.");
  }

  // find depth format
  const std::vector<VkFormat> preferredDepthFormats = {VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT};

  Image::depthFormat = VK_FORMAT_UNDEFINED;
  for(const auto& format : preferredDepthFormats) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(device->physicalDevice()->handle, format, &properties);
    if(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      Image::depthFormat = format;

      Image::depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
      if(format != VK_FORMAT_D32_SFLOAT) { Image::depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT; }

      break;
    }
  }
  if(Image::depthFormat == VK_FORMAT_UNDEFINED) {
    throw std::runtime_error("No prefered depth image format available.");
  }

  return;
}

Image::Image(VkImage image, VkImageView view, VkExtent2D extent) {
  handle = image;
  this->view = view;

  _extent = extent;
  _format = VK_FORMAT_UNDEFINED;
  _aspect = VK_IMAGE_ASPECT_COLOR_BIT;

  _allocation = {};
  _info = {};
  _info.pName = SWAPCHAIN_IMAGE_TAG.c_str();
  _channelAmount = {4};
  _usage = 0;
}

Image::Image(const Swapchain& swapchain, ImageType type, VkImageUsageFlags usageFlags) {
  std::shared_ptr<Device> deviceSptr = getShared(_device);
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);

  _extent = {swapchain.extent.width, swapchain.extent.height};
  _usage = usageFlags;

  switch(type) {
    case COLOR: {
      _aspect = Image::colorAspect;
      _format = Image::colorFormat;
      _channelAmount = 4;
    } break;

    case DEPTH: {
      _aspect = Image::depthAspect;
      _format = Image::depthFormat;
      if(_format == VK_FORMAT_D32_SFLOAT)
        _channelAmount = 1;
      else
        _channelAmount = 2;
      break;
    } break;
  }

  VkImageCreateInfo createInfo = getCreateInfo();
  VmaAllocationCreateInfo allocCreateInfo = getAllocationInfo();
  VK_CHECK(vmaCreateImage(allocatorSptr->handle, &createInfo, &allocCreateInfo, &handle,
    &_allocation, &_info));

  VkImageViewCreateInfo viewCreateInfo = getViewCreateInfo();
  VK_CHECK(vkCreateImageView(deviceSptr->handle, &viewCreateInfo, nullptr, &view));
}

Image::Image(std::string path) {
  std::shared_ptr<Device> deviceSptr = getShared(_device);
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);
  std::shared_ptr<ImmediateSubmit> immediateSubmitSptr = getShared(_immediateSubmit);

  int width, height;
  stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &_channelAmount, STBI_rgb_alpha);
  _channelAmount = 4;

  if(!pixels) { throw std::runtime_error("Failed to find image file."); }

  _extent.width = static_cast<uint32_t>(width);
  _extent.height = static_cast<uint32_t>(height);
  _format = VK_FORMAT_R8G8B8A8_SRGB;
  _usage =
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  _aspect = VK_IMAGE_ASPECT_COLOR_BIT;

  // VkPhysicalDeviceLimits limits = _devicePtr->limits();
  // if (_extent.width > limits.maxImageDimension2D || _extent.height >
  // limits.maxImageDimension2D) {
  //   //todo: implement resizing -> then resize image to whatever
  //   //stbir_resize_uint8_srgb()
  // }

  size_t imageSize = static_cast<size_t>(_extent.width * _extent.height * _channelAmount);
  Buffer* stagingBuffer =
    new Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  void* data;
  vmaMapMemory(allocatorSptr->handle, stagingBuffer->allocation(), &data);
  memcpy(data, pixels, imageSize);
  vmaUnmapMemory(allocatorSptr->handle, stagingBuffer->allocation());

  stbi_image_free(pixels);

  VkImageCreateInfo createInfo = getCreateInfo();
  VmaAllocationCreateInfo allocationCreateInfo = getAllocationInfo();
  VK_CHECK(vmaCreateImage(allocatorSptr->handle, &createInfo, &allocationCreateInfo, &handle,
    &_allocation, &_info));

  view = VK_NULL_HANDLE;
  immediateSubmitSptr->submit([&stagingBuffer, this](std::shared_ptr<CommandBuffer> commandBuffer) {
    this->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    stagingBuffer->copyToImage(commandBuffer, *this);
    this->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  });
  delete stagingBuffer;

  VkImageViewCreateInfo viewCreateInfo = getViewCreateInfo();
  VK_CHECK(vkCreateImageView(deviceSptr->handle, &viewCreateInfo, nullptr, &view));
}

Image::Image(int width, int height, Color color) {
  std::shared_ptr<Device> deviceSptr = getShared(_device);
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);
  std::shared_ptr<ImmediateSubmit> immediateSubmitSptr = getShared(_immediateSubmit);

  if(width < 1) width = 1;
  if(height < 1) height = 1;

  _channelAmount = 4;

  std::vector<uint8_t> pixels{};
  int pixelQty = width * height;
  for(int i = 0; i < pixelQty; i++){
    pixels.push_back(255 * color.r);
    pixels.push_back(255 * color.g);
    pixels.push_back(255 * color.b);
    pixels.push_back(255 * color.a);
  }

  _extent.width = (uint32_t)width;
  _extent.height = (uint32_t)height;
  _format = VK_FORMAT_R8G8B8A8_SRGB;
  _usage =
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  _aspect = VK_IMAGE_ASPECT_COLOR_BIT;

  size_t imageSize = static_cast<size_t>(_extent.width * _extent.height * _channelAmount);
  Buffer* stagingBuffer =
    new Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  void* data;
  vmaMapMemory(allocatorSptr->handle, stagingBuffer->allocation(), &data);
  memcpy(data, pixels.data(), imageSize);
  vmaUnmapMemory(allocatorSptr->handle, stagingBuffer->allocation());

  VkImageCreateInfo createInfo = getCreateInfo();
  VmaAllocationCreateInfo allocationCreateInfo = getAllocationInfo();
  VK_CHECK(vmaCreateImage(allocatorSptr->handle, &createInfo, &allocationCreateInfo, &handle,
    &_allocation, &_info));

  view = VK_NULL_HANDLE;
  immediateSubmitSptr->submit([&stagingBuffer, this](std::shared_ptr<CommandBuffer> commandBuffer) {
    this->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    stagingBuffer->copyToImage(commandBuffer, *this);
    this->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  });
  delete stagingBuffer;

  VkImageViewCreateInfo viewCreateInfo = getViewCreateInfo();
  VK_CHECK(vkCreateImageView(deviceSptr->handle, &viewCreateInfo, nullptr, &view));
}

Image::~Image() {
  // swapchain owned images are destroyed by swapchain
  if(_info.pName && SWAPCHAIN_IMAGE_TAG.compare(_info.pName) == 0) return;

  std::shared_ptr<Device> deviceSptr = getShared(_device);
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);

  vkDestroyImageView(deviceSptr->handle, view, nullptr);

  if(_info.deviceMemory == VK_NULL_HANDLE) {
    vkDestroyImage(deviceSptr->handle, handle, nullptr);
  } else {
    vmaDestroyImage(allocatorSptr->handle, handle, _allocation);
  }
}

void Image::transitionLayout(std::shared_ptr<CommandBuffer> commandBuffer,
  VkImageLayout newLayout) {
  VkImageMemoryBarrier2 imageBarrier {};
  imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  // imageBarrier.pNext = nullptr;

  imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
  imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

  imageBarrier.oldLayout = _layout;
  imageBarrier.newLayout = newLayout;
  _layout = newLayout;

  // imageBarrier.srcQueueFamilyIndex = 0;
  // imageBarrier.dstQueueFamilyIndex = 0;

  imageBarrier.image = handle;
  imageBarrier.subresourceRange = getSubresourceRange();

  VkDependencyInfo depInfo {};
  depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  // depInfo.pNext = nullptr;
  // depInfo.dependencyFlags = 0;

  // depInfo.memoryBarrierCount = 0;
  // depInfo.pBufferMemoryBarriers = nullptr;

  // depInfo.bufferMemoryBarrierCount = 0;
  // depInfo.pBufferMemoryBarriers = nullptr;

  depInfo.imageMemoryBarrierCount = 1;
  depInfo.pImageMemoryBarriers = &imageBarrier;

  vkCmdPipelineBarrier2(commandBuffer->handle, &depInfo);
}

void Image::transitionFormat(std::shared_ptr<CommandBuffer> commandBuffer, VkFormat newFormat) {
  std::shared_ptr<Device> deviceSptr = getShared(_device);
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);
  std::shared_ptr<ImmediateSubmit> immediateSubmitSptr = getShared(_immediateSubmit);

  Image* stagingImg = new Image(*this);
  VkImageLayout layout = _layout;
  _layout = VK_IMAGE_LAYOUT_UNDEFINED;

  vkDestroyImageView(deviceSptr->handle, view, nullptr);
  vmaDestroyImage(allocatorSptr->handle, handle, _allocation);

  _format = newFormat;
  VkImageCreateInfo createInfo = getCreateInfo();
  VmaAllocationCreateInfo allocationCreateInfo = getAllocationInfo();
  VK_CHECK(vmaCreateImage(allocatorSptr->handle, &createInfo, &allocationCreateInfo, &handle,
    &_allocation, &_info));

  VkImageViewCreateInfo viewCreateInfo = getViewCreateInfo();
  VK_CHECK(vkCreateImageView(deviceSptr->handle, &viewCreateInfo, nullptr, &view));

  immediateSubmitSptr->submit(
    [&layout, &stagingImg, this](std::shared_ptr<CommandBuffer> commandBuffer) {
      this->transitionLayout(commandBuffer, layout);
      stagingImg->blitTo(commandBuffer, std::shared_ptr<Image>(this));
    });

  delete stagingImg;
}

VkImageCreateInfo Image::getCreateInfo() const {
  VkImageCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  // info.pNext = nullptr;
  // info.flags = 0;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.format = _format;
  info.extent = {_extent.width, _extent.height, 1};
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.usage = _usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  // info.queueFamilyIndexCount = 0;
  // info.pQueueFamilyIndices = nullptr;
  info.initialLayout = _layout;

  return info;
}

VkImageViewCreateInfo Image::getViewCreateInfo() const {
  VkImageViewCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  // info.pNext = nullptr;
  // info.flags = 0;
  info.image = handle;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = _format;
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.subresourceRange = getSubresourceRange();

  return info;
}

VmaAllocationCreateInfo Image::getAllocationInfo() const {
  VmaAllocationCreateInfo info {};
  // info.flags = 0;
  info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  // info.preferredFlags = 0;
  // info.memoryTypeBits = 0;
  info.pool = VK_NULL_HANDLE;
  // info.pUserData = nullptr;
  // info.priority = 0;

  return info;
}

VkRenderingAttachmentInfo Image::getRenderingAttachmentInfo(const VkClearValue& clear,
  bool doClear) const {
  VkRenderingAttachmentInfo attachmentInfo {};
  attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  // colorAttachment.pNext = nullptr;

  attachmentInfo.imageView = view;
  attachmentInfo.imageLayout = _layout;

  attachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
  attachmentInfo.resolveImageView = VK_NULL_HANDLE;
  attachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  attachmentInfo.loadOp = doClear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
  attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  if(doClear) { attachmentInfo.clearValue = clear; }

  // delete clear;

  return attachmentInfo;
}

RenderingInfoData Image::getRenderingInfo(const Image& color, const Image& depth,
  const VkClearValue& clear, bool doClear) {
  RenderingInfoData data;

  data.colorAttachments.push_back(color.getRenderingAttachmentInfo(clear, true));
  data.depthAttachment = depth.getRenderingAttachmentInfo(VkClearValue {1, 0}, true);

  data.info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  data.info.pNext = nullptr;
  data.info.flags = 0;
  data.info.renderArea.extent = color._extent;
  data.info.renderArea.offset = {0, 0};
  data.info.layerCount = 1;
  data.info.viewMask = 0;
  data.info.colorAttachmentCount = (uint32_t) data.colorAttachments.size();
  data.info.pColorAttachments = data.colorAttachments.data();
  data.info.pDepthAttachment = &data.depthAttachment;
  data.info.pStencilAttachment = nullptr;

  return data;
}

VkImageSubresourceRange Image::getSubresourceRange() const {
  VkImageSubresourceRange range {};
  range.aspectMask = _aspect;
  // range.baseMipLevel = 0;
  range.levelCount = 1;
  // range.baseArrayLayer = 0;
  range.layerCount = 1;

  return range;
}

VkImageSubresourceLayers Image::getSubresourceLayers() const {
  VkImageSubresourceLayers layers {};
  layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  // layers.mipLevel = 0;
  // layers.baseArrayLayer = 0;
  layers.layerCount = 1;

  return layers;
}

void Image::blitTo(std::shared_ptr<CommandBuffer> commandBuffer,
  std::shared_ptr<Image> image) const {
  VkImageBlit2 blitRegion {};
  blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
  // blitRegion.pNext = nullptr;

  blitRegion.srcSubresource = getSubresourceLayers();

  // blitRegion.srcOffsets[0].x = 0;
  // blitRegion.srcOffsets[0].y = 0;
  // blitRegion.srcOffsets[0].z = 0;
  blitRegion.srcOffsets[1].x = _extent.width;
  blitRegion.srcOffsets[1].y = _extent.height;
  blitRegion.srcOffsets[1].z = 1;

  blitRegion.dstSubresource = image->getSubresourceLayers();

  // blitRegion.dstOffsets[0].x = 0;
  // blitRegion.dstOffsets[0].y = 0;
  // blitRegion.dstOffsets[0].z = 0;
  blitRegion.dstOffsets[1].x = image->_extent.width;
  blitRegion.dstOffsets[1].y = image->_extent.height;
  blitRegion.dstOffsets[1].z = 1;

  VkBlitImageInfo2 blitInfo {};
  blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
  // blitInfo.pNext = nullptr;
  blitInfo.srcImage = handle;
  blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  blitInfo.dstImage = image->handle;
  blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  blitInfo.regionCount = 1;
  blitInfo.pRegions = &blitRegion;
  blitInfo.filter = VK_FILTER_LINEAR;

  vkCmdBlitImage2(commandBuffer->handle, &blitInfo);
}

void Image::copyTo(std::shared_ptr<CommandBuffer> commandBuffer,
  std::shared_ptr<Image> image) const {
  VkImageCopy region {};
  region.srcSubresource = getSubresourceLayers();
  // region.srcOffset = 0;
  region.dstSubresource = image->getSubresourceLayers();
  // region.dstOffset = 0;
  region.extent = {_extent.width, _extent.height, 1};

  vkCmdCopyImage(commandBuffer->handle, handle, _layout, image->handle, image->_layout, 1, &region);
}

void Image::copyToBuffer(std::shared_ptr<CommandBuffer> commandBuffer,
  std::shared_ptr<Buffer> buffer) const {
  VkBufferImageCopy region {};
  // region.bufferOffset;
  // region.bufferRowLength;
  // region.bufferImageHeight;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  // region.imageSubresource.baseArrayLayer;
  region.imageSubresource.layerCount = 1;
  // region.imageOffset;
  region.imageExtent = {_extent.width, _extent.height, 1};

  vkCmdCopyImageToBuffer(commandBuffer->handle, handle, _layout, buffer->handle, 1, &region);
}

void Image::save(const char* path) {
  std::shared_ptr<Allocator> allocatorSptr = getShared(_allocator);
  std::shared_ptr<ImmediateSubmit> immediateSubmitSptr = getShared(_immediateSubmit);

  uint32_t imageSize = _extent.width * _extent.height * _channelAmount;
  if(_format != VK_FORMAT_R8G8B8A8_SRGB) imageSize *= 2;
  imageSize += 1;
  Buffer* staging =
    new Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

  immediateSubmitSptr->submit([&staging, this](std::shared_ptr<CommandBuffer> cmd) {
    VkImageLayout oldLayout = this->layout();
    if(oldLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
      this->transitionLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    this->copyToBuffer(cmd, std::shared_ptr<Buffer>(staging));

    if(oldLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) this->transitionLayout(cmd, oldLayout);
  });

  void* data;
  vmaMapMemory(allocatorSptr->handle, staging->allocation(), &data);
  delete staging;

  if(stbi_write_png(path, _extent.height, _extent.width, _channelAmount, data,
       _extent.width * _channelAmount) == 0) {
    throw std::runtime_error("Failed to save image.");
  }
}

void Image::clear(std::shared_ptr<CommandBuffer> commandBuffer, Color color) {
  VkClearColorValue clearColor = {color.r, color.g, color.b, color.a};
  VkImageSubresourceRange clearRange = getSubresourceRange();
  vkCmdClearColorImage(commandBuffer->handle, handle, _layout, &clearColor, 1, &clearRange);
}
