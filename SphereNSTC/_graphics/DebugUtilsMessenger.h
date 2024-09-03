#pragma once

#include "VulkanUtils.h"

#ifdef VKDEBUG

class Instance;

class DebugUtilsMessenger {
private:
  VkInstance* _instancePtr{ nullptr };

public:
  VkDebugUtilsMessengerEXT handle{ VK_NULL_HANDLE };

  static VkDebugUtilsMessengerCreateInfoEXT& getCreateInfo();

  DebugUtilsMessenger() {}
  DebugUtilsMessenger(Instance* instance);
  ~DebugUtilsMessenger();
};

#endif // VKDEBUG