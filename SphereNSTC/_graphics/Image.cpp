#include "Image.h"

#include "Device.h"
#include "Allocator.h"
#include "CommandBuffer.h"

#include "stb/stb_image.h"

Image::Image(VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent,
  VkImageAspectFlags aspectFlags, VkImageLayout layout) {
  this->extent = {
    extent.width,
    extent.height
  };
  this->format = format;

  VkImageCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  //createInfo.pNext = nullptr;
  //createInfo.flags = 0;
  createInfo.imageType = VK_IMAGE_TYPE_2D;
  createInfo.format = format;
  createInfo.extent = {
    extent.width,
    extent.height,
    1
  };
  createInfo.mipLevels = 1;
  createInfo.arrayLayers = 1;
  createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  createInfo.usage = usageFlags;
  createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //createInfo.queueFamilyIndexCount = 0;
  //createInfo.pQueueFamilyIndices = nullptr;
  createInfo.initialLayout = _layout = layout;

  VmaAllocationCreateInfo allocCreateInfo{};
  //allocCreateInfo.flags = 0;
  allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  //allocCreateInfo.preferredFlags = 0;
  //allocCreateInfo.memoryTypeBits = 0;
  allocCreateInfo.pool = VK_NULL_HANDLE;
  //allocCreateInfo.pUserData = nullptr;
  //allocCreateInfo.priority = 0;

  VK_CHECK(vmaCreateImage(_allocatorPtr, &createInfo, &allocCreateInfo, &handle, &_allocation, nullptr));

  VkImageViewCreateInfo viewCreateInfo{};
  viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  //viewCreateInfo.pNext = nullptr;
  //viewCreateInfo.flags = 0;
  viewCreateInfo.image = handle;
  viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewCreateInfo.format = format;
  //viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  //viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  //viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  //viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
  //viewCreateInfo.subresourceRange.baseMipLevel = 0;
  viewCreateInfo.subresourceRange.levelCount = 1;
  //viewCreateInfo.subresourceRange.baseArrayLayer = 0;
  viewCreateInfo.subresourceRange.layerCount = 1;

  VK_CHECK(vkCreateImageView(_devicePtr, &viewCreateInfo, nullptr, &view));
}

Image::Image(const char* path) {
  int width, height, channels;
  stbi_uc* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);

  if (!pixels) {
    throw new Exception("Failed to find image file.");
  }

  extent.width = static_cast<uint32_t>(width);
  extent.height = static_cast<uint32_t>(height);

  format = VK_FORMAT_R8G8B8A8_SRGB;

  VkImageCreateInfo createInfo = getCreateInfo();

  vmaCreateImage(_allocatorPtr, &createInfo, )
}

Image::~Image() {
  vkDestroyImageView(_devicePtr, view, nullptr);

  if (_allocatorPtr == nullptr) {
    vkDestroyImage(_devicePtr, handle, nullptr);
  } else {
    vmaDestroyImage(_allocatorPtr, handle, _allocation);
  }
}

void Image::transitionLayout(CommandBuffer* commandBuffer, VkImageLayout newLayout) const {
  VkImageMemoryBarrier2 imageBarrier{};
  imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  //imageBarrier.pNext = nullptr;

  imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
  imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

  imageBarrier.oldLayout = _layout;
  imageBarrier.newLayout = newLayout;

  //imageBarrier.srcQueueFamilyIndex = 0;
  //imageBarrier.dstQueueFamilyIndex = 0;

  imageBarrier.image = handle;
  imageBarrier.subresourceRange.aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
  //imageBarrier.subresourceRange.baseMipLevel = 0;
  imageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  //imageBarrier.subresourceRange.baseArrayLayer = 0;
  imageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

  VkDependencyInfo depInfo{};
  depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  //depInfo.pNext = nullptr;
  //depInfo.dependencyFlags = 0;

  //depInfo.memoryBarrierCount = 0;
  //depInfo.pBufferMemoryBarriers = nullptr;

  //depInfo.bufferMemoryBarrierCount = 0;
  //depInfo.pBufferMemoryBarriers = nullptr;

  depInfo.imageMemoryBarrierCount = 1;
  depInfo.pImageMemoryBarriers = &imageBarrier;

  vkCmdPipelineBarrier2(commandBuffer->handle, &depInfo);
}

VkImageCreateInfo Image::getCreateInfo(VkImageUsageFlags usage) {
  VkImageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.format = format;
  info.extent = {
    extent.width,
    extent.height,
    1
  };
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //info.queueFamilyIndexCount = 0;
  //info.pQueueFamilyIndices = nullptr;
  info.initialLayout = _layout;

  return info
}

VkRenderingAttachmentInfo Image::getRenderingAttachmentInfo(VkClearValue* clear) const {
  VkRenderingAttachmentInfo colorAttachment{};
  colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  //colorAttachment.pNext = nullptr;

  colorAttachment.imageView = view;
  colorAttachment.imageLayout = _layout;

  colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
  colorAttachment.resolveImageView = VK_NULL_HANDLE;
  colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  if (clear) {
    colorAttachment.clearValue = *clear;
  }

  return colorAttachment;
}

void Image::init(Device* device, Allocator* allocator) {
  _devicePtr = device->handle;
  _allocatorPtr = allocator->handle;
}

VkRenderingInfo Image::getRenderingInfo(VkClearValue* clear) const {
  VkRenderingInfo renderInfo{};
  renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  //renderInfo.pNext = nullptr;
  //renderInfo.flags = 0;
  renderInfo.renderArea.extent = extent;
  renderInfo.renderArea.offset = { 0, 0 };
  renderInfo.layerCount = 1;
  //renderInfo.viewMask = 0;
  renderInfo.colorAttachmentCount = 1;
  renderInfo.pColorAttachments = &getRenderingAttachmentInfo(clear);
  //renderInfo.pDepthAttachment = nullptr;
  //renderInfo.pStencilAttachment = nullptr;

  return renderInfo;
}

void Image::copyTo(CommandBuffer* commandBuffer, Image* image) const {
  VkImageBlit2 blitRegion{};
  blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
  //blitRegion.pNext = nullptr;

  blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  //blitRegion.srcSubresource.baseArrayLayer = 0;
  blitRegion.srcSubresource.layerCount = 1;
  //blitRegion.srcSubresource.mipLevel = 0;

  //blitRegion.srcOffsets[0].x = 0;
  //blitRegion.srcOffsets[0].y = 0;
  //blitRegion.srcOffsets[0].z = 0;
  blitRegion.srcOffsets[1].x = extent.width;
  blitRegion.srcOffsets[1].y = extent.height;
  blitRegion.srcOffsets[1].z = 1;

  blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  //blitRegion.dstSubresource.baseArrayLayer = 0;
  blitRegion.dstSubresource.layerCount = 1;
  //blitRegion.dstSubresource.mipLevel = 0;

  //blitRegion.dstOffsets[0].x = 0;
  //blitRegion.dstOffsets[0].y = 0;
  //blitRegion.dstOffsets[0].z = 0;
  blitRegion.dstOffsets[1].x = image->extent.width;
  blitRegion.dstOffsets[1].y = image->extent.height;
  blitRegion.dstOffsets[1].z = 1;

  VkBlitImageInfo2 blitInfo{};
  blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
  //blitInfo.pNext = nullptr;
  blitInfo.srcImage = handle;
  blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  blitInfo.dstImage = image->handle;
  blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  blitInfo.regionCount = 1;
  blitInfo.pRegions = &blitRegion;
  blitInfo.filter = VK_FILTER_LINEAR;

  vkCmdBlitImage2(commandBuffer->handle, &blitInfo);
}
