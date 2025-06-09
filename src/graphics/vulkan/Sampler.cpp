#include "Sampler.h"

#include "Device.h"

VkSamplerCreateInfo Sampler::getCreateInfo() const {
  VkSamplerCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.magFilter = VK_FILTER_LINEAR;
  info.minFilter = VK_FILTER_LINEAR;
  info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.mipLodBias = 0.0f;
  info.anisotropyEnable = VK_FALSE;
  info.maxAnisotropy = 1.0f;
  info.compareEnable = VK_FALSE;
  info.compareOp = VK_COMPARE_OP_ALWAYS;
  info.minLod = 0.0f;
  info.maxLod = 0.0f;
  info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = VK_FALSE;

  return info;
}

const VkSampler& Sampler::getHandle() const { return _handle; }

Sampler::Sampler(std::shared_ptr<Device> device) {
  _device = device;

  VkSamplerCreateInfo createInfo = getCreateInfo();
  VK_CHECK(vkCreateSampler(device->handle, &createInfo, nullptr, &_handle));
}

Sampler::~Sampler() { vkDestroySampler(_device->handle, _handle, nullptr); }
