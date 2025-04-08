#pragma once

#include "VulkanUtils.h"

#ifdef VKDEBUG

class Instance;

class DebugUtilsMessenger {
private:
  std::shared_ptr<Instance> _instance;

public:
  VkDebugUtilsMessengerEXT handle;

  static VkDebugUtilsMessengerCreateInfoEXT getCreateInfo();

  DebugUtilsMessenger(std::shared_ptr<Instance> instance);
  ~DebugUtilsMessenger();
};

#endif // VKDEBUG