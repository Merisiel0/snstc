#include "VulkanObjects.h"

#include "Utils.h"
#include "VulkanUtils.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include <vector>
#include <map>
#include <string>

Window::Window(const char* title) {
  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  handle = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    dm.w,
    dm.h,
    SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);
}

Window::~Window() {
  SDL_DestroyWindow(handle);
}

#ifdef VKDEBUG
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

VkDebugUtilsMessengerCreateInfoEXT& DebugUtilsMessenger::getCreateInfo() {
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

VkApplicationInfo& Instance::getApplicationInfo(const char* applicationName, uint32_t applicationVersion, const char* engineName, uint32_t engineVersion) {
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

VkInstanceCreateInfo& Instance::getCreateInfo(VkApplicationInfo appInfo, std::vector<const char*>& requiredLayers, std::vector<const char*>& requiredExtensions) {
  VkInstanceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info.flags = 0;
  info.pApplicationInfo = &appInfo;
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
    throw new Exception("Some requested instance layers are missing.");
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
    throw new Exception("Some requested instance extensions are missing.");
  }

  VkApplicationInfo appInfo = getApplicationInfo(applicationName, applicationVersion, engineName, engineVersion);

  VkInstanceCreateInfo createInfo = getCreateInfo(appInfo, requiredLayers, requiredExtensions);

#ifdef VKDEBUG
  VkDebugUtilsMessengerCreateInfoEXT debugInfo = DebugUtilsMessenger::getCreateInfo();
  createInfo.pNext = &debugInfo;
#endif // VKDEBUG

  VK_CHECK(vkCreateInstance(&createInfo, nullptr, &handle));
}

Instance::~Instance() {
  vkDestroyInstance(handle, nullptr);
}

PhysicalDeviceQueueFamilyIndexes& PhysicalDevice::findQueueFamilyIndexes(const VkPhysicalDevice& physicalDevice) {
  uint32_t queueFamilyPropertyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

  PhysicalDeviceQueueFamilyIndexes queueFamilyIndexes;

  for (uint32_t i = 0; i < queueFamilyPropertyCount; i++) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndexes.graphicsFamilyIndex = i;
      break;
    }
  }

  if (queueFamilyIndexes.graphicsFamilyIndex.has_value()) {
    for (uint32_t i = 0; i < queueFamilyPropertyCount; i++) {
      if (i != queueFamilyIndexes.graphicsFamilyIndex && (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
        queueFamilyIndexes.computeFamilyIndex = i;
        break;
      }
    }
  }

  return queueFamilyIndexes;
}

int PhysicalDevice::rate(VkPhysicalDevice physicalDevice, std::vector<const char*> requiredExtensions) {
  int score{ 0 };

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  uint32_t minorVersion = VK_VERSION_MINOR(properties.apiVersion);
  if (VK_VERSION_MINOR(properties.apiVersion) < 3) return 0;

  switch (properties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    score += 100;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    score += 10;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    score += 1;
    break;
  default:
    return 0;
  }

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
  rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
  //rayTracingPipelineFeatures.pNext = nullptr;

  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
  accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
  accelStructFeatures.pNext = &rayTracingPipelineFeatures;

  VkPhysicalDeviceVulkan13Features vk13Features{};
  vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  vk13Features.pNext = &accelStructFeatures;

  VkPhysicalDeviceVulkan12Features vk12Features{};
  vk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  vk12Features.pNext = &vk13Features;

  VkPhysicalDeviceFeatures2 features{};
  features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
  features.pNext = &vk12Features;

  vkGetPhysicalDeviceFeatures2KHR(physicalDevice, &features);

  if (vk12Features.bufferDeviceAddress == VK_FALSE
    || vk13Features.dynamicRendering == VK_FALSE
    || vk13Features.synchronization2 == VK_FALSE
    || accelStructFeatures.accelerationStructure == VK_FALSE
    || rayTracingPipelineFeatures.rayTracingPipeline == VK_FALSE) return 0;

  PhysicalDeviceQueueFamilyIndexes queueFamilyIndexes = findQueueFamilyIndexes(physicalDevice);
  if (!queueFamilyIndexes.isComplete()) return 0;

  uint32_t deviceExtensionPropertyCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionPropertyCount, nullptr);
  std::vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionPropertyCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionPropertyCount, deviceExtensionProperties.data());

  size_t requiredExtensionsFound{ 0 };
  for (const auto& deviceExtensionProperty : deviceExtensionProperties) {
    for (const auto& requiredExtension : requiredExtensions) {
      if (strcmp(deviceExtensionProperty.extensionName, requiredExtension) == 0) {
        requiredExtensionsFound++;
        score++;
        break;
      }
    }
  }
  if (requiredExtensionsFound < requiredExtensions.size()) return 0;

  return score;
}

PhysicalDevice::PhysicalDevice(Instance* instance, std::vector<const char*> requiredExtensions) {
  size_t requiredExtensionsFound{ 0 };

  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(instance->handle, &physicalDeviceCount, nullptr);
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance->handle, &physicalDeviceCount, physicalDevices.data());

  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto& physicalDevice : physicalDevices) {
    int score = rate(physicalDevice, requiredExtensions);
    candidates.insert(std::make_pair(score, physicalDevice));
  }

  if (candidates.rbegin()->first > 0) {
    handle = candidates.rbegin()->second;
    queueFamilies = findQueueFamilyIndexes(handle);

    //Core.RTPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    //VkPhysicalDeviceProperties2 physicalDeviceProperties{};
    //physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    //physicalDeviceProperties.pNext = &Core.RTPipelineProperties;
    ////physicalDeviceProperties.properties;
    //vkGetPhysicalDeviceProperties2(Core.physicalDevice, &physicalDeviceProperties);
  } else {
    throw Exception("Failed to find a suitable GPU.");
  }
}

Queue::Queue(Device* device, uint32_t familyIndex) {
  this->familyIndex = familyIndex;
  vkGetDeviceQueue(device->handle, familyIndex, 0, &handle);
}

VkDeviceCreateInfo& Device::getCreateInfo(VkPhysicalDeviceFeatures2 features, std::vector<VkDeviceQueueCreateInfo>& queues, std::vector<const char*>& extensions) {
  VkDeviceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pNext = &features;
  info.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
  info.pQueueCreateInfos = queues.data();
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();
  //info.pEnabledFeatures

  return info;
}

Device::Device(PhysicalDevice* physicalDevice, std::vector<const char*> extensions) {
  std::vector<uint32_t> queueFamilyIndexes = physicalDevice->queueFamilies.getIndexes();

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;

  for (size_t i = 0; i < queueFamilyIndexes.size(); i++) {
    VkDeviceQueueCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    //createInfo.pNext;
    //createInfo.flags;
    createInfo.queueFamilyIndex = queueFamilyIndexes[i];
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(createInfo);
  }

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
  rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
  //rayTracingPipelineFeatures.pNext = nullptr;
  rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;

  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
  accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
  accelStructFeatures.pNext = &rayTracingPipelineFeatures;
  accelStructFeatures.accelerationStructure = VK_TRUE;

  VkPhysicalDeviceVulkan13Features vk13Features{};
  vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  vk13Features.pNext = &accelStructFeatures;
  vk13Features.synchronization2 = VK_TRUE;
  vk13Features.dynamicRendering = VK_TRUE;

  VkPhysicalDeviceVulkan12Features vk12Features{};
  vk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  vk12Features.pNext = &vk13Features;
  vk12Features.bufferDeviceAddress = VK_TRUE;

  VkPhysicalDeviceFeatures2 features{};
  features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
  features.pNext = &vk12Features;

  VkDeviceCreateInfo createInfo = getCreateInfo(features, queueCreateInfos, extensions);

  vkCreateDevice(physicalDevice->handle, &createInfo, nullptr, &handle);

  graphicsQueue = new Queue(this, physicalDevice->queueFamilies.graphicsFamilyIndex.value());
  computeQueue = new Queue(this, physicalDevice->queueFamilies.computeFamilyIndex.value());
}

Device::~Device() {
  delete graphicsQueue;
  delete computeQueue;

  vkDestroyDevice(handle, nullptr);
}

void Device::waitIdle() const {
  VK_CHECK(vkDeviceWaitIdle(handle));
}

VmaAllocatorCreateInfo& Allocator::getCreateInfo(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device) {
  VmaAllocatorCreateInfo info{};
  info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  info.physicalDevice = physicalDevice;
  info.device = device;
  info.instance = instance;
  info.vulkanApiVersion = VK_API_VERSION_1_3;

  return info;
}

Allocator::Allocator(Instance* instance, PhysicalDevice* physicalDevice, Device* device) {
  VmaAllocatorCreateInfo createInfo = getCreateInfo(instance->handle, physicalDevice->handle, device->handle);
  VK_CHECK(vmaCreateAllocator(&createInfo, &handle));
}

Allocator::~Allocator() {
  vmaDestroyAllocator(handle);
}

VkSemaphoreCreateInfo Semaphore::getCreateInfo() {
  VkSemaphoreCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;

  return info;
}

Semaphore::Semaphore(Device* device) {
  this->_devicePtr = &device->handle;

  VkSemaphoreCreateInfo createInfo = getCreateInfo();
  VK_CHECK(vkCreateSemaphore(device->handle, &createInfo, nullptr, &handle));
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(*_devicePtr, handle, nullptr);
}

VkFenceCreateInfo& Fence::getCreateInfo(bool signaled) {
  VkFenceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  //info.pNext = nullptr;
  info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  return info;
}

Fence::Fence(Device* device, bool signaled) {
  this->_devicePtr = &device->handle;

  VkFenceCreateInfo createInfo = getCreateInfo(signaled);
  VK_CHECK(vkCreateFence(device->handle, &createInfo, nullptr, &handle));
}

Fence::~Fence() {
  vkDestroyFence(*_devicePtr, handle, nullptr);
}

void Fence::reset() {
  VK_CHECK(vkResetFences(*_devicePtr, 1, &handle));
}

void Fence::wait() {
  VK_CHECK(vkWaitForFences(*_devicePtr, 1, &handle, true, 9999999999));
}

VkCommandPoolCreateInfo& CommandPool::getCreateInfo(VkDevice device, uint32_t queueFamily) {
  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  //info.pNext = nullptr;
  info.queueFamilyIndex = queueFamily;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  return info;
}

CommandPool::CommandPool(Device* device, uint32_t queueFamilyIndex) {
  this->_devicePtr = &device->handle;

  VkCommandPoolCreateInfo createInfo = getCreateInfo(device->handle, queueFamilyIndex);
  VK_CHECK(vkCreateCommandPool(device->handle, &createInfo, nullptr, &handle));
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(*_devicePtr, handle, nullptr);
}

VkCommandBufferAllocateInfo& CommandBuffer::getAllocateInfo(VkCommandPool commandPool) {
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  //info.pNext = nullptr;
  info.commandPool = commandPool;
  info.commandBufferCount = 1;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  return info;
}

VkCommandBufferBeginInfo& CommandBuffer::getBeginInfo(VkCommandBufferUsageFlags flags) const {
  VkCommandBufferBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  //info.pNext = nullptr;
  info.flags = flags;
  //info.pInheritanceInfo = nullptr;

  return info;
}

CommandBuffer::CommandBuffer(Device* device, CommandPool* commandPool) {
  VkCommandBufferAllocateInfo allocationInfo = getAllocateInfo(commandPool->handle);
  VK_CHECK(vkAllocateCommandBuffers(device->handle, &allocationInfo, &handle));
}

void CommandBuffer::reset() const {
  VK_CHECK(vkResetCommandBuffer(handle, 0));
}

void CommandBuffer::begin() const {
  VK_CHECK(vkBeginCommandBuffer(handle, &getBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)));
}

void CommandBuffer::end() const {
  VK_CHECK(vkEndCommandBuffer(handle));
}

void CommandBuffer::submitToQueue(Queue* queue, Fence* fence) const {
  VkCommandBufferSubmitInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  //info.pNext = nullptr;
  info.commandBuffer = handle;
  //info.deviceMask = 0;

  VkSubmitInfo2 info2{};
  info2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  //info2.pNext = nullptr;
  //info2.flags = 0;
  //info2.waitSemaphoreInfoCount;
  //info2.pWaitSemaphoreInfos;
  //info2.signalSemaphoreInfoCount;
  //info2.pSignalSemaphoreInfos;
  info2.commandBufferInfoCount = 1;
  info2.pCommandBufferInfos = &info;

  VK_CHECK(vkQueueSubmit2(queue->handle, 1, &info2, fence->handle));
}

Frame::Frame(Device* device) {
  commandPool = new CommandPool(device, device->graphicsQueue->familyIndex);
  commandBuffer = new CommandBuffer(device, commandPool);

  swapchainSemaphore = new Semaphore(device);
  renderSemaphore = new Semaphore(device);
  renderFence = new Fence(device);
}

Frame::~Frame() {
  delete commandPool;
  delete commandBuffer;

  delete swapchainSemaphore;
  delete renderSemaphore;
  delete renderFence;
}

Image::Image(Device* device, Allocator* allocator, VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent, VkImageAspectFlags aspectFlags) {
  this->_devicePtr = &device->handle;
  this->_allocatorPtr = &allocator->handle;

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
  createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VmaAllocationCreateInfo allocCreateInfo{};
  //allocCreateInfo.flags = 0;
  allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  //allocCreateInfo.preferredFlags = 0;
  //allocCreateInfo.memoryTypeBits = 0;
  allocCreateInfo.pool = VK_NULL_HANDLE;
  //allocCreateInfo.pUserData = nullptr;
  //allocCreateInfo.priority = 0;

  VK_CHECK(vmaCreateImage(allocator->handle, &createInfo, &allocCreateInfo, &handle, &_allocation, nullptr));

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

  VK_CHECK(vkCreateImageView(device->handle, &viewCreateInfo, nullptr, &view));
}

Image::~Image() {
  vkDestroyImageView(*_devicePtr, view, nullptr);

  if (_allocatorPtr == nullptr) {
    vkDestroyImage(*_devicePtr, handle, nullptr);
  } else {
    vmaDestroyImage(*_allocatorPtr, handle, _allocation);
  }
}

Swapchain::Swapchain(Window* window, Instance* instance, PhysicalDevice* physicalDevice, Device* device) {
  this->_instancePtr = &instance->handle;
  this->_devicePtr = &device->handle;

  frames = new Frame * [FRAME_OVERLAP];
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    frames[i] = new Frame(device);
  }

  SDL_BCHECK(SDL_Vulkan_CreateSurface(window->handle, instance->handle, &_surface));

  VkSurfaceCapabilitiesKHR capabilities{};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->handle, _surface, &capabilities));

  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle, _surface, &surfaceFormatCount, nullptr);
  std::vector<VkSurfaceFormatKHR> availableSurfaceFormats(surfaceFormatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->handle, _surface, &surfaceFormatCount, availableSurfaceFormats.data());

  const std::vector<VkFormat> preferredFormats = {
      VK_FORMAT_R32G32B32A32_UINT,
      VK_FORMAT_R16G16B16A16_UNORM,
      VK_FORMAT_R16G16B16A16_UINT,
      VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_R8G8B8A8_UINT
  };

  const std::vector<VkColorSpaceKHR> preferredColorSpaces = {
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    VK_COLOR_SPACE_HDR10_HLG_EXT,
    VK_COLOR_SPACE_HDR10_ST2084_EXT
  };

  VkSurfaceFormatKHR surfaceFormat{};
  for (const auto& colorSpace : preferredColorSpaces) {
    for (const auto& format : preferredFormats) {
      for (const auto& availableSurfaceFormat : availableSurfaceFormats) {
        if (availableSurfaceFormat.format == format && availableSurfaceFormat.colorSpace == colorSpace) {
          surfaceFormat = availableSurfaceFormat;
          goto surfaceFormatChosen;
        }
      }
    }
  }
surfaceFormatChosen:

  uint32_t presentModeCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle, _surface, &presentModeCount, nullptr));
  std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->handle, _surface, &presentModeCount, availablePresentModes.data()));

  const std::vector<VkPresentModeKHR> preferredPresentModes = {
      VK_PRESENT_MODE_MAILBOX_KHR,
      VK_PRESENT_MODE_FIFO_KHR,
      VK_PRESENT_MODE_IMMEDIATE_KHR
  };
  VkPresentModeKHR presentMode{};

  for (const auto& preferredPresentMode : preferredPresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (preferredPresentMode == availablePresentMode) {
        presentMode = preferredPresentMode;
        goto presentModeChosen;
      }
    }
  }
presentModeChosen:

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  //createInfo.pNext = nullptr;
  //createInfo.flags = 0;
  createInfo.surface = _surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent = capabilities.currentExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //createInfo.queueFamilyIndexCount = 0;
  //createInfo.pQueueFamilyIndices = nullptr;
  createInfo.preTransform = capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  //createInfo.oldSwapchain = nullptr;

  VK_CHECK(vkCreateSwapchainKHR(device->handle, &createInfo, nullptr, &handle));

  // get swapchain images
  _imageCount = new uint32_t;
  VK_CHECK(vkGetSwapchainImagesKHR(device->handle, handle, _imageCount, nullptr));
  std::vector<VkImage> vkImages(*_imageCount);
  VK_CHECK(vkGetSwapchainImagesKHR(device->handle, handle, _imageCount, vkImages.data()));

  _images = new Image * [*_imageCount];
  // get swapchain image views
  std::vector<VkImageView> imageViews(*_imageCount);
  for (uint32_t i = 0; i < *_imageCount; i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //createInfo.pNext = nullptr;
    //createInfo.flags = 0;
    createInfo.image = vkImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = imageFormat;
    //createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    //createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    //createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    //createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    //createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device->handle, &createInfo, nullptr, &imageViews[i]));

    _images[i] = new Image(device->handle, vkImages[i], imageViews[i]);
  }
}

Swapchain::~Swapchain() {
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    delete frames[i];
  }
  delete[] frames;

  for (uint32_t i = 0; i < *_imageCount; i++) {
    vkDestroyImageView(*_devicePtr, _images[i]->view, nullptr);
  }
  delete _imageCount;

  vkDestroySwapchainKHR(*_devicePtr, handle, nullptr);
  vkDestroySurfaceKHR(*_instancePtr, _surface, nullptr);
}

ImmediateSubmit::ImmediateSubmit(Device* device) {
  _commandPool = new CommandPool(device, device->graphicsQueue->familyIndex);
  _commandBuffer = new CommandBuffer(device, _commandPool);
  _fence = new Fence(device);
  _queue = device->graphicsQueue;
}

ImmediateSubmit::~ImmediateSubmit() {
  delete _commandPool;
  delete _commandBuffer;
  delete _fence;
}

void ImmediateSubmit::submit(std::function<void(VkCommandBuffer cmd)>&& function) {
  _fence->reset();
  _commandBuffer->reset();

  _commandBuffer->begin();

  function(_commandBuffer->handle);

  _commandBuffer->end();

  _commandBuffer->submitToQueue(_queue, _fence);

  _fence->wait();
}

VkBufferCreateInfo& Buffer::getCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage) {
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;
  info.size = size;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //info.queueFamilyIndexCount = 0;
  //info.pQueueFamilyIndices = nullptr;

  return info;
}

VmaAllocationCreateInfo& Buffer::getAllocationCreateInfo(VmaMemoryUsage usage) {
  VmaAllocationCreateInfo info{};
  info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  info.usage = usage;
  //info.requiredFlags = 0;
  //info.preferredFlags = 0;
  //info.memoryTypeBits = 0;
  //info.pool = VK_NULL_HANDLE;
  //info.pUserData = nullptr;
  //info.priority = 0;

  return info;
}

VkBufferDeviceAddressInfo& Buffer::getDeviceAddressInfo() const {
  VkBufferDeviceAddressInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  info.pNext = nullptr;
  info.buffer = handle;

  return info;
}

Buffer::Buffer(Device* device, Allocator* allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
  _allocatorPtr = &allocator->handle;

  usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  VkBufferCreateInfo createInfo = getCreateInfo(size, usage);
  VmaAllocationCreateInfo vmaInfo = getAllocationCreateInfo(memoryUsage);

  VK_CHECK(vmaCreateBuffer(allocator->handle, &createInfo, &vmaInfo, &handle, &_allocation, &_info));

  VkBufferDeviceAddressInfo deviceAdressInfo = getDeviceAddressInfo();
  address = vkGetBufferDeviceAddress(device->handle, &deviceAdressInfo);
}

Buffer::~Buffer() {
  vmaDestroyBuffer(*_allocatorPtr, handle, _allocation);
}

VkAccelerationStructureCreateInfoKHR& IAccelerationStructure::getCreateInfo(VkDeviceSize size, VkAccelerationStructureTypeKHR type) {
  VkAccelerationStructureCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  //info.pNext;
  //info.createFlags;
  info.buffer = _memory->handle;
  //info.offset;
  info.size = size;
  info.type = type;
  //info.deviceAddress;

  return info;
}

//VkAccelerationStructureBuildGeometryInfoKHR& IAccelerationStructure::getBuildGeometryInfo(VkAccelerationStructureTypeKHR type,
//  bool complete, VkBuildAccelerationStructureModeKHR mode) {
//  VkAccelerationStructureBuildGeometryInfoKHR info{};
//  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
//  //info.pNext;
//  info.type = type;
//  info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
//  info.mode = mode;
//  //info.srcAccelerationStructure;
//  info.dstAccelerationStructure = handle;
//  info.geometryCount = 1;
//  info.pGeometries = &_geometry;
//  //info.ppGeometries;
//  info.scratchData.deviceAddress = _scratch == nullptr ? 0 : _scratch->address;
//
//  return info;
//}
//
//VkAccelerationStructureBuildSizesInfoKHR& IAccelerationStructure::getBuildSizesInfo(Device* device) {
//  VkAccelerationStructureBuildGeometryInfoKHR geometryInfo = getBuildGeometryInfo(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);
//
//  uint32_t primitiveCount = geometryInfo.geometryCount * (geometryInfo.pGeometries->geometry.triangles.maxVertex + 1);
//
//  VkAccelerationStructureBuildSizesInfoKHR info{};
//  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
//  vkGetAccelerationStructureBuildSizesKHR(device->handle, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
//    &geometryInfo, &primitiveCount, &info);
//
//  return info;
//}
//
//VkAccelerationStructureBuildRangeInfoKHR& IAccelerationStructure::getBuildRangeInfo(uint32_t primitiveCount) {
//  VkAccelerationStructureBuildRangeInfoKHR info{};
//  info.primitiveCount = primitiveCount;
//  //info.primitiveOffset;
//  info.firstVertex = 0;
//  //info.transformOffset;
//
//  return info;
//}

//void IAccelerationStructure::uploadToDevice(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, VkAccelerationStructureTypeKHR type) {
//  _devicePtr = &device->handle;
//
//  VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo = getBuildSizesInfo(device);
//
//  _scratch = new Buffer(device, allocator, buildSizesInfo.buildScratchSize,
//    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
//    VMA_MEMORY_USAGE_GPU_ONLY);
//
//  _memory = new Buffer(device, allocator, buildSizesInfo.accelerationStructureSize,
//    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);
//
//  VkAccelerationStructureCreateInfoKHR createInfo = getCreateInfo(buildSizesInfo.accelerationStructureSize,
//    type);
//
//  vkCreateAccelerationStructureKHR(device->handle, &createInfo, nullptr, &handle);
//
//  immediateSubmit->submit([&](VkCommandBuffer cmd) {
//    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = getBuildGeometryInfo(type);
//    const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &getBuildRangeInfo();
//    const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
//    vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, cpcvkp);
//    });
//}

IAccelerationStructure::~IAccelerationStructure() {
  vkDestroyAccelerationStructureKHR(*_devicePtr, handle, nullptr);
  delete _memory;
  delete _scratch;
}

BLAS::BLAS(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
  _devicePtr = &device->handle;

  // --- GEOMETRY ---
  _geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  //_geometry.pNext;
  _geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  _geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

  _vertices = new Buffer(device, allocator, immediateSubmit, vertices,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
    VMA_MEMORY_USAGE_GPU_ONLY);
  _indices = new Buffer(device, allocator, immediateSubmit, indices,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
    VMA_MEMORY_USAGE_GPU_ONLY);

  _geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  //_geometry.geometry.triangles.pNext;
  _geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  _geometry.geometry.triangles.vertexData.deviceAddress = _vertices->address;
  _geometry.geometry.triangles.vertexStride = sizeof(Vertex);
  _geometry.geometry.triangles.maxVertex = static_cast<uint32_t>(vertices.size() - 1);
  _geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
  _geometry.geometry.triangles.indexData.deviceAddress = _indices->address;
  //_geometry.geometry.triangles.transformData; //TODO: implement transforms

  // --- BUILD GEOMETRY INFO ---
  _buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  //_buildGeometryInfo.pNext;
  _buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  _buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  _buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  //_buildGeometryInfo.srcAccelerationStructure;
  _buildGeometryInfo.geometryCount = 1;
  _buildGeometryInfo.pGeometries = &_geometry;
  //_buildGeometryInfo.ppGeometries;

  // --- BUILD SIZES ---
  uint32_t primitiveCount = static_cast<uint32_t>(vertices.size() / 3);

  _buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  vkGetAccelerationStructureBuildSizesKHR(device->handle, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
    &_buildGeometryInfo, &primitiveCount, &_buildSizesInfo);

  _scratch = new Buffer(device, allocator, _buildSizesInfo.buildScratchSize + 256,
    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  _buildGeometryInfo.scratchData.deviceAddress = _scratch->address + 256 - (_scratch->address % 256);

  _memory = new Buffer(device, allocator, _buildSizesInfo.accelerationStructureSize,
    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);

  VkAccelerationStructureCreateInfoKHR createInfo = getCreateInfo(_buildSizesInfo.accelerationStructureSize,
    VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);

  vkCreateAccelerationStructureKHR(device->handle, &createInfo, nullptr, &handle);
  _buildGeometryInfo.dstAccelerationStructure = handle;

  // --- BUILD RANGE INFO ---
  _buildRangeInfo.primitiveCount = primitiveCount;
  //_buildRangeInfo.primitiveOffset;
  _buildRangeInfo.firstVertex = 0;
  //_buildRangeInfo.transformOffset;

  immediateSubmit->submit([&](VkCommandBuffer cmd) {
    const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &_buildRangeInfo;
    const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
    vkCmdBuildAccelerationStructuresKHR(cmd, 1, &_buildGeometryInfo, cpcvkp);
    });
}

BLAS::~BLAS() {
  delete _vertices;
  delete _indices;
}

VkAccelerationStructureInstanceKHR BLAS::getInstance(glm::mat4 transform) {
  VkAccelerationStructureInstanceKHR instance{};
  //instance.instanceCustomIndex;
  //instance.mask;
  //instance.instanceShaderBindingTableRecordOffset;
  //instance.flags;
  //instance.accelerationStructureReference;

  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 4; x++) {
      instance.transform.matrix[y][x] = transform[y][x];
    }
  }

  return instance;
}

TLAS::TLAS(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<VkAccelerationStructureInstanceKHR> instances) {
  _devicePtr = &device->handle;

  // --- GEOMETRY ---
  _geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  //_geometry.pNext;
  _geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  _geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

  _instances.resize(instances.size());
  std::vector<VkDeviceAddress> addresses(instances.size());
  for (size_t i = 0; i < instances.size(); i++) {
    std::vector<VkAccelerationStructureInstanceKHR> instance = { instances[i] };
    _instances[i] = new Buffer(device, allocator, immediateSubmit, instance,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
      VMA_MEMORY_USAGE_GPU_ONLY);
    addresses[i] = _instances[i]->address;
  }
  _instancesBuffer = new Buffer(device, allocator, immediateSubmit, _instances,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
    VMA_MEMORY_USAGE_GPU_ONLY);

  _geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  //_geometry.geometry.instances.pNext;
  _geometry.geometry.instances.arrayOfPointers = VK_FALSE;
  _geometry.geometry.instances.data.deviceAddress = _instancesBuffer->address;

  // --- BUILD GEOMETRY INFO ---
  _buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  //_buildGeometryInfo.pNext;
  _buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  _buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  _buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  //_buildGeometryInfo.srcAccelerationStructure;
  _buildGeometryInfo.geometryCount = static_cast<uint32_t>(instances.size());
  _buildGeometryInfo.pGeometries = &_geometry;
  //_buildGeometryInfo.ppGeometries;

  // --- BUILD SIZES ---
  uint32_t primitiveCount = static_cast<uint32_t>(instances.size());

  _buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  vkGetAccelerationStructureBuildSizesKHR(device->handle, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
    &_buildGeometryInfo, &primitiveCount, &_buildSizesInfo);

  _scratch = new Buffer(device, allocator, _buildSizesInfo.buildScratchSize + 256,
    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  _buildGeometryInfo.scratchData.deviceAddress = _scratch->address + 256 - (_scratch->address % 256);

  _memory = new Buffer(device, allocator, _buildSizesInfo.accelerationStructureSize,
    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);

  VkAccelerationStructureCreateInfoKHR createInfo = getCreateInfo(_buildSizesInfo.accelerationStructureSize,
    VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);

  vkCreateAccelerationStructureKHR(device->handle, &createInfo, nullptr, &handle);
  _buildGeometryInfo.dstAccelerationStructure = handle;

  // --- BUILD RANGE INFO ---
  _buildRangeInfo.primitiveCount = primitiveCount;
  //_buildRangeInfo.primitiveOffset;
  _buildRangeInfo.firstVertex = 0;
  //_buildRangeInfo.transformOffset;

  immediateSubmit->submit([&](VkCommandBuffer cmd) {
    const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &_buildRangeInfo;
    const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
    vkCmdBuildAccelerationStructuresKHR(cmd, 1, &_buildGeometryInfo, cpcvkp);
    });
}

TLAS::~TLAS() {
  for (auto& buffer : _instances) {
    delete buffer;
  }

  delete _instancesBuffer;
}

Shader::Shader(Device* device, const char* path, VkShaderStageFlagBits stage) {
  _devicePtr = &device->handle;
  _stage = stage;

  std::vector<uint32_t> data = readFileBytes(path);

  VkShaderModuleCreateInfo moduleCreateInfo{};
  moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  //moduleCreateInfo.pNext;
  //moduleCreateInfo.flags;
  moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
  moduleCreateInfo.pCode = data.data();

  vkCreateShaderModule(device->handle, &moduleCreateInfo, nullptr, &handle);
}

Shader::~Shader() {
  vkDestroyShaderModule(*_devicePtr, handle, nullptr);
}

VkPipelineShaderStageCreateInfo Shader::getStageCreateInfo() const {
  VkPipelineShaderStageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  //info.pNext;
  //info.flags;
  info.stage = _stage;
  info.module = handle;
  info.pName = "main";
  //info.pSpecializationInfo;

  return info;
}
