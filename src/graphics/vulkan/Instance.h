#pragma once

#include "VulkanUtils.h"

class Instance {
  private:
  VkApplicationInfo getApplicationInfo(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion) const;
  VkInstanceCreateInfo getCreateInfo(VkApplicationInfo* appInfo, std::vector<const char*>& requiredLayers, std::vector<const char*>& requiredExtensions) const;

  public:
  VkInstance handle;

  Instance() { handle = VK_NULL_HANDLE; }

  Instance(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion,
    std::vector<const char*> requiredLayers, std::vector<const char*> requiredExtensions);

  ~Instance();
};