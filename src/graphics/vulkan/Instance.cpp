#include "Instance.h"

#include "DebugUtilsMessenger.h"

VkApplicationInfo Instance::getApplicationInfo(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion) const {
  VkApplicationInfo info{};
  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  //info.pNext = nullptr;
  info.pApplicationName = applicationName;
  info.applicationVersion = applicationVersion;
  info.pEngineName = engineName;
  info.engineVersion = engineVersion;
  info.apiVersion = VK_API_VERSION_1_3;

  return info;
}

VkInstanceCreateInfo Instance::getCreateInfo(VkApplicationInfo* appInfo, std::vector<const char*>& requiredLayers, std::vector<const char*>& requiredExtensions) const {
  VkInstanceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  //info.flags = 0;
  info.pApplicationInfo = appInfo;
  info.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
  info.ppEnabledLayerNames = requiredLayers.data();
  info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
  info.ppEnabledExtensionNames = requiredExtensions.data();

  return info;
}

Instance::Instance(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion,
  std::vector<const char*> requiredLayers, std::vector<const char*> requiredExtensions) {
  uint32_t layerPropertyCount;
  VK_CHECK(vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr));
  std::vector<VkLayerProperties> layerProperties(layerPropertyCount);
  VK_CHECK(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data()));

  size_t requiredLayersFound{ 0 };
  for (const auto& layerProperty : layerProperties) {
    for (const auto& requiredLayer : requiredLayers) {
      if (strcmp(layerProperty.layerName, requiredLayer) == 0) {
        requiredLayersFound++;
        break;
      }
    }
  }
  if (requiredLayersFound < requiredLayers.size()) {
    throw std::runtime_error("Some requested instance layers are missing.");
  }

  uint32_t extensionPropertyCount;
  VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr));
  std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
  VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensionProperties.data()));

  size_t requiredExtensionsFound{ 0 };
  for (const auto& extensionProperty : extensionProperties) {
    for (const auto& requiredExtension : requiredExtensions) {
      if (strcmp(extensionProperty.extensionName, requiredExtension) == 0) {
        requiredExtensionsFound++;
        break;
      }
    }
  }
  if (requiredExtensionsFound < requiredExtensions.size()) {
    throw std::runtime_error("Some requested instance extensions are missing.");
  }

  VkApplicationInfo appInfo = getApplicationInfo(applicationName, applicationVersion, engineName, engineVersion);
  VkInstanceCreateInfo createInfo = getCreateInfo(&appInfo, requiredLayers, requiredExtensions);

#ifdef VKDEBUG
  VkDebugUtilsMessengerCreateInfoEXT debugInfo = DebugUtilsMessenger::getCreateInfo();
  createInfo.pNext = &debugInfo;
#endif // VKDEBUG

  VK_CHECK(vkCreateInstance(&createInfo, nullptr, &handle));
}

Instance::~Instance() {
  vkDestroyInstance(handle, nullptr);
}