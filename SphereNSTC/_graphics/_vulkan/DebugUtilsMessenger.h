#pragma once

#include "VulkanUtils.h"

#ifdef VKDEBUG

class Instance;

class DebugUtilsMessenger {
private:
  VkInstance* _instancePtr;

public:
  VkDebugUtilsMessengerEXT handle;

  static VkDebugUtilsMessengerCreateInfoEXT getCreateInfo();

  DebugUtilsMessenger(Instance* instance);
  ~DebugUtilsMessenger();
};

#endif // VKDEBUG