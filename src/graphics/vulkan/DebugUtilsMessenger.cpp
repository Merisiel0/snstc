#include "DebugUtilsMessenger.h"

#ifdef VKDEBUG

#include "Instance.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  std::cerr << "Severity: ";
  switch (messageSeverity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    std::cerr << "verbose";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    std::cerr << "info";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    std::cerr << "warning";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    std::cerr << "error";
    break;
  default:
    std::cerr << "NULL";
    break;
  }

  std::cerr << " | Type: ";
  switch (messageType) {
  case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
    std::cerr << "general";
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
    std::cerr << "validation";
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
    std::cerr << "performance";
    break;
  default:
    std::cerr << "NULL";
    break;
  }

  std::cerr << "\nValidation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessenger::getCreateInfo() {
  VkDebugUtilsMessengerCreateInfoEXT info{};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  //info.pNext = nullptr;
  //info.flags = 0;
  info.messageSeverity =
    /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |*/
    /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |*/
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType =
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = debugCallback;
  //info.pUserData = nullptr;

  return info;
}

DebugUtilsMessenger::DebugUtilsMessenger(Instance* instance) {
  this->_instancePtr = &instance->handle;

  VkDebugUtilsMessengerCreateInfoEXT createInfo = getCreateInfo();
  VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance->handle, &createInfo, nullptr, &handle));
}

DebugUtilsMessenger::~DebugUtilsMessenger() {
  vkDestroyDebugUtilsMessengerEXT(*_instancePtr, handle, nullptr);
}

#endif // VKDEBUG