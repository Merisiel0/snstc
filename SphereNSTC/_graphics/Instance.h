#pragma once

#include "VulkanUtils.h"

class Instance {
private:
  VkApplicationInfo& getApplicationInfo(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion);
  VkInstanceCreateInfo& getCreateInfo(VkApplicationInfo appInfo, std::vector<const char*>& requiredLayers, std::vector<const char*>& requiredExtensions);

public:
  VkInstance handle{ VK_NULL_HANDLE };

  Instance() {}
  Instance(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion,
    std::vector<const char*> requiredLayers, std::vector<const char*> requiredExtensions);
  ~Instance();
};