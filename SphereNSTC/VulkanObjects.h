#pragma once

#include "VulkanUtils.h"
#include "FunctionQueue.h"

#include <vector>
#include <optional>

class Window {
public:
  SDL_Window* handle;

  Window() : handle{ nullptr } {}
  Window(const char* title);
  ~Window();
};

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

struct PhysicalDeviceQueueFamilyIndexes {
  std::optional<uint32_t> graphicsFamilyIndex;
  std::optional<uint32_t> computeFamilyIndex;

  bool isComplete() const {
    return graphicsFamilyIndex.has_value() && computeFamilyIndex.has_value();
  }

  std::vector<uint32_t> getIndexes() {
    std::vector<uint32_t> indexes;

    if (graphicsFamilyIndex.has_value()) indexes.push_back(graphicsFamilyIndex.value());
    if (computeFamilyIndex.has_value()) indexes.push_back(computeFamilyIndex.value());

    return indexes;
  }
};

class PhysicalDevice {
private:
  int rate(VkPhysicalDevice physicalDevice, std::vector<const char*> requiredExtensions);
  PhysicalDeviceQueueFamilyIndexes& findQueueFamilyIndexes(const VkPhysicalDevice& physicalDevice);

public:
  VkPhysicalDevice handle{ VK_NULL_HANDLE };
  PhysicalDeviceQueueFamilyIndexes queueFamilies;

  PhysicalDevice() {}
  PhysicalDevice(Instance* instance, std::vector<const char*> requiredExtensions);
  ~PhysicalDevice() {};
};

class Queue;

class Device {
private:
  VkDeviceCreateInfo& getCreateInfo(VkPhysicalDeviceFeatures2 features, std::vector<VkDeviceQueueCreateInfo>& queues, std::vector<const char*>& extensions);

public:
  VkDevice handle{ VK_NULL_HANDLE };
  Queue* graphicsQueue{ nullptr };
  Queue* computeQueue{ nullptr };

  Device() {}
  Device(PhysicalDevice* physicalDevice, std::vector<const char*> extensions);
  ~Device();

  void waitIdle() const;
};

class Queue {
public:
  VkQueue handle{ VK_NULL_HANDLE };
  uint32_t familyIndex{ UINT32_MAX };

  Queue() {}
  Queue(Device* device, uint32_t familyIndex);
  ~Queue() {};
};

class Allocator {
private:
  VmaAllocatorCreateInfo& getCreateInfo(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);

public:
  VmaAllocator handle{ VK_NULL_HANDLE };

  Allocator() {}
  Allocator(Instance* instance, PhysicalDevice* physicalDevice, Device* device);
  ~Allocator();
};

class Semaphore {
private:
  VkDevice* _devicePtr{ nullptr };

  VkSemaphoreCreateInfo getCreateInfo();

public:
  VkSemaphore handle{ VK_NULL_HANDLE };

  Semaphore() {}
  Semaphore(Device* device);
  ~Semaphore();
};

class Fence {
private:
  VkDevice* _devicePtr{ nullptr };

  VkFenceCreateInfo& getCreateInfo(bool signaled);

public:
  VkFence handle{ VK_NULL_HANDLE };

  Fence() {}
  Fence(Device* device, bool signaled = false);
  ~Fence();

  void reset();
  void wait();
};

class CommandPool {
private:
  VkDevice* _devicePtr{ nullptr };

  VkCommandPoolCreateInfo& getCreateInfo(VkDevice device, uint32_t queueFamily);

public:
  VkCommandPool handle{ VK_NULL_HANDLE };

  CommandPool() {}
  CommandPool(Device* device, uint32_t queueFamilyIndex);
  ~CommandPool();
};

class CommandBuffer {
private:
  VkCommandBufferAllocateInfo& getAllocateInfo(VkCommandPool commandPool);
  VkCommandBufferBeginInfo& getBeginInfo(VkCommandBufferUsageFlags flags) const;

public:
  VkCommandBuffer handle{ VK_NULL_HANDLE };

  CommandBuffer() {}
  CommandBuffer(Device* device, CommandPool* commandPool);
  ~CommandBuffer() {};

  void reset() const;
  void begin() const;
  void end() const;
  void submitToQueue(Queue* queue, Fence* fence) const;
};

class Frame {
public:
  CommandPool* commandPool{ nullptr };
  CommandBuffer* commandBuffer{ nullptr };

  Semaphore* swapchainSemaphore{ nullptr };
  Semaphore* renderSemaphore{ nullptr };
  Fence* renderFence{ nullptr };

  Frame() {}
  Frame(Device* device);
  ~Frame();
};

class Image {
private:
  VkDevice* _devicePtr{ nullptr };
  VmaAllocator* _allocatorPtr{ nullptr };
  VmaAllocation _allocation{ VK_NULL_HANDLE };

public:
  VkImage handle{ VK_NULL_HANDLE };
  VkImageView view{ VK_NULL_HANDLE };
  VkExtent2D extent{ 0,0 };
  VkFormat format{ VK_FORMAT_UNDEFINED };

  Image() {}
  Image(VkDevice device, VkImage image, VkImageView view)
    : _devicePtr{ &device }, handle{ image }, view{ view } {}
  Image(Device* device, Allocator* allocator, VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent, VkImageAspectFlags aspectFlags);
  ~Image();
};

class Swapchain {
private:
  VkInstance* _instancePtr{ nullptr };
  VkDevice* _devicePtr{ nullptr };

  VkSurfaceKHR _surface{ VK_NULL_HANDLE };
  Image** _images{ nullptr };
  uint32_t* _imageCount{ nullptr };

  static const unsigned int FRAME_OVERLAP = 2;

public:
  VkSwapchainKHR handle{ VK_NULL_HANDLE };
  VkFormat imageFormat{ VK_FORMAT_UNDEFINED };
  VkExtent2D extent{ 0,0 };

  uint32_t frameNumber{ 0 };
  Frame** frames{ nullptr };
  Frame* getCurrentFrame() const { return frames[frameNumber % FRAME_OVERLAP]; }

  Swapchain() {}
  Swapchain(Window* window, Instance* instance, PhysicalDevice* physicalDevice, Device* device);
  ~Swapchain();
};

class ImmediateSubmit {
private:
  CommandPool* _commandPool{ nullptr };
  CommandBuffer* _commandBuffer{ nullptr };
  Fence* _fence{ nullptr };
  Queue* _queue{ nullptr };

public:
  ImmediateSubmit() {};
  ImmediateSubmit(Device* device);
  ~ImmediateSubmit();

  void submit(std::function<void(VkCommandBuffer cmd)>&& function);
};

class Buffer {
private:
  VmaAllocator* _allocatorPtr{ nullptr };
  VmaAllocation _allocation{ VK_NULL_HANDLE };
  VmaAllocationInfo _info{};

public:
  VkBuffer handle{ VK_NULL_HANDLE };
  VkDeviceAddress address{};

  VkBufferCreateInfo& getCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage);
  VmaAllocationCreateInfo& getAllocationCreateInfo(VmaMemoryUsage usage);
  VkBufferDeviceAddressInfo& getDeviceAddressInfo() const;

  Buffer() {}

  Buffer(Device* device, Allocator* allocator, const VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

  template<typename T>
  Buffer(Device* device, Allocator* allocator, ImmediateSubmit* immmediateSubmit, std::vector<T> data, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
    : Buffer(device, allocator, data.size() * sizeof(T), usage, memoryUsage) {
    const size_t bufferSize = data.size() * sizeof(T);
    Buffer* staging = new Buffer(device, allocator, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    memcpy(staging->_allocation->GetMappedData(), data.data(), bufferSize);

    immmediateSubmit->submit([&](VkCommandBuffer cmd) {
      VkBufferCopy copy{};
      //copy.dstOffset = 0;
      //copy.srcOffset = 0;
      copy.size = bufferSize;

      vkCmdCopyBuffer(cmd, staging->handle, handle, 1, &copy);
      });

    delete staging;
  }

  ~Buffer();
};

class IAccelerationStructure {
protected:
  VkDevice* _devicePtr{ nullptr };
  VkAccelerationStructureGeometryKHR _geometry{};
  Buffer* _memory{ nullptr };
  Buffer* _scratch{ nullptr };

  VkAccelerationStructureBuildGeometryInfoKHR _buildGeometryInfo{};
  VkAccelerationStructureBuildSizesInfoKHR _buildSizesInfo{};
  VkAccelerationStructureBuildRangeInfoKHR _buildRangeInfo{};

  VkAccelerationStructureCreateInfoKHR& getCreateInfo(VkDeviceSize size, VkAccelerationStructureTypeKHR type);

  /*VkAccelerationStructureBuildGeometryInfoKHR& getBuildGeometryInfo(VkAccelerationStructureTypeKHR type, bool complete = true,
    VkBuildAccelerationStructureModeKHR mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR);

  VkAccelerationStructureBuildSizesInfoKHR& getBuildSizesInfo(Device* device);

  VkAccelerationStructureBuildRangeInfoKHR& getBuildRangeInfo(uint32_t primitiveCount = 1);*/

  //void uploadToDevice(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, VkAccelerationStructureTypeKHR type);

public:
  VkAccelerationStructureKHR handle{ VK_NULL_HANDLE };

  IAccelerationStructure() {}
  ~IAccelerationStructure();
};

class BLAS : public IAccelerationStructure {
private:
  Buffer* _vertices{ nullptr };
  Buffer* _indices{ nullptr };

public:
  BLAS() {}
  BLAS(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
  ~BLAS();

  VkAccelerationStructureInstanceKHR getInstance(glm::mat4 transform);
};

class TLAS : IAccelerationStructure {
private:
  std::vector<Buffer*> _instances{};
  Buffer* _instancesBuffer{ nullptr };

public:
  TLAS() {}
  TLAS(Device* device, Allocator* allocator, ImmediateSubmit* immediateSubmit, std::vector<VkAccelerationStructureInstanceKHR> instances);
  ~TLAS();
};

class Shader {
private:
  VkDevice* _devicePtr{ nullptr };
  VkShaderStageFlagBits _stage{};

public:
  VkShaderModule handle{ VK_NULL_HANDLE };

  Shader() {};
  Shader(Device* device, const char* path, VkShaderStageFlagBits stage);
  ~Shader();

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;
};