#include "Graphics.h"

#include "VulkanObjects.h"
#include "VulkanUtils.h"
#include "ECS.h"

#include <vector>
#include <optional>
#include <array>

//struct Pipeline {
//	VkPipeline handle{ VK_NULL_HANDLE };
//	VkPipelineLayout layout{ VK_NULL_HANDLE };
//};

struct Core {

  ImmediateSubmit* immediateSubmit;

#ifdef VKDEBUG
  DebugUtilsMessenger* debugMessenger;
#endif // VKDEBUG

  Instance* instance;
  PhysicalDevice* physicalDevice;
  Device* device;
  Allocator* allocator;

  VkPhysicalDeviceRayTracingPipelinePropertiesKHR RTPipelineProperties{};

  Window* window;
  Swapchain* swapchain;
  Image* drawImage;

  BLAS* triangleBLAS;
  TLAS* worldTLAS;
  //Pipeline rayTracingPipeline;

  std::vector<Vertex> vertices;
  Core(GraphicsInitInfo initInfo) {
    window = new Window("SphereNSTC");

    std::vector<const char*> instanceLayers{
  #ifdef VKDEBUG
      "VK_LAYER_KHRONOS_validation"
  #endif // VKDEBUG
    };

    std::vector<const char*> instanceExtensions{
  #ifdef VKDEBUG
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  #endif // VKDEBUG

      VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    };

    uint32_t sdlExtensionCount;
    SDL_BCHECK(SDL_Vulkan_GetInstanceExtensions(window->handle, &sdlExtensionCount, nullptr));
    std::vector<const char*> sdlExtensions(sdlExtensionCount);
    SDL_BCHECK(SDL_Vulkan_GetInstanceExtensions(window->handle, &sdlExtensionCount, sdlExtensions.data()));
    for (uint32_t i = 0; i < sdlExtensionCount; i++) {
      instanceExtensions.push_back(sdlExtensions[i]);
    }

    instance = new Instance(initInfo.applicationName, initInfo.applicationVersion, initInfo.engineName, initInfo.engineVersion, instanceLayers, instanceExtensions);

    fetchExtensionFunctionPointers(instance->handle);

#ifdef VKDEBUG
    debugMessenger = new DebugUtilsMessenger(instance);
#endif // VKDEBUG

    std::vector<const char*> deviceExtensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
      VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
      VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
    };
    physicalDevice = new PhysicalDevice(instance, deviceExtensions);
    device = new Device(physicalDevice, deviceExtensions);
    allocator = new Allocator(instance, physicalDevice, device);

    immediateSubmit = new ImmediateSubmit(device);

    swapchain = new Swapchain(window, instance, physicalDevice, device);

    drawImage = new Image(device, allocator, swapchain->imageFormat,
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      swapchain->extent, VK_IMAGE_ASPECT_COLOR_BIT);

    // --- Temporary Code ---

    std::vector<Vertex> vertices(3);
    vertices[0].position = { 0.25f, 0.25f, 0.0f };
    vertices[1].position = { 0.75f, 0.25f, 0.0f };
    vertices[2].position = { 0.50f, 0.75f, 0.0f };

    std::vector<uint32_t> indices = { 0, 1, 2 };

    triangleBLAS = new BLAS(device, allocator, immediateSubmit, vertices, indices);

    worldTLAS = new TLAS(device, allocator, immediateSubmit, { triangleBLAS->getInstance(glm::mat4(1.0f)) });

    Shader* rayGen = new Shader(device, "resources/shaders/rayGenShader.rgen", VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    Shader* rayChit = new Shader(device, "resources/shaders/rayChitShader.rchit", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    Shader* rayMiss = new Shader(device, "resources/shaders/rayMissShader.rmiss", VK_SHADER_STAGE_MISS_BIT_KHR);
  }

  ~Core() {
    device->waitIdle();

    delete worldTLAS;
    delete triangleBLAS;

    delete drawImage;
    delete swapchain;

    delete immediateSubmit;

    delete allocator;
    delete device;

#ifdef VKDEBUG
    delete debugMessenger;
#endif // VKDEBUG

    delete instance;
    delete window;
  }
};

static Core* core{ nullptr };
//
//VkDeviceAddress TLAS::getAddress() const {
//	VkAccelerationStructureDeviceAddressInfoKHR info{};
//	info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
//	//info.pNext;
//	info.accelerationStructure = handle;
//
//	return vkGetAccelerationStructureDeviceAddressKHR(Core.device, &info);
//}
//
//static VulkanImage createImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent2D extent, VkImageAspectFlags aspectFlags) {
//	VulkanImage image{};
//	image.extent = {
//		extent.width,
//		extent.height
//	};
//	image.format = format;
//
//	VkImageCreateInfo createInfo{};
//	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//	//createInfo.pNext = nullptr;
//	//createInfo.flags = 0;
//	createInfo.imageType = VK_IMAGE_TYPE_2D;
//	createInfo.format = image.format;
//	createInfo.extent = {
//		image.extent.width,
//		image.extent.height,
//		1
//	};
//	createInfo.mipLevels = 1;
//	createInfo.arrayLayers = 1;
//	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//	createInfo.usage = usageFlags;
//	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//	//createInfo.queueFamilyIndexCount = 0;
//	//createInfo.pQueueFamilyIndices = nullptr;
//	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//
//	VmaAllocationCreateInfo allocCreateInfo{};
//	//allocCreateInfo.flags = 0;
//	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//	allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//	//allocCreateInfo.preferredFlags = 0;
//	//allocCreateInfo.memoryTypeBits = 0;
//	allocCreateInfo.pool = VK_NULL_HANDLE;
//	//allocCreateInfo.pUserData = nullptr;
//	//allocCreateInfo.priority = 0;
//
//	VK_CHECK(vmaCreateImage(Core.allocator, &createInfo, &allocCreateInfo, &image.handle, &image.allocation, nullptr));
//
//	VkImageViewCreateInfo viewCreateInfo{};
//	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	viewCreateInfo.pNext = nullptr;
//	viewCreateInfo.flags = 0;
//	viewCreateInfo.image = image.handle;
//	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//	viewCreateInfo.format = image.format;
//	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
//	viewCreateInfo.subresourceRange.baseMipLevel = 0;
//	viewCreateInfo.subresourceRange.levelCount = 1;
//	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
//	viewCreateInfo.subresourceRange.layerCount = 1;
//
//	VK_CHECK(vkCreateImageView(Core.device, &viewCreateInfo, nullptr, &image.view));
//
//	return image;
//}
//
//static void getPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, std::vector<uint32_t> code,
//	VkShaderModule& shaderModule, VkPipelineShaderStageCreateInfo& shaderStageCreateInfo) {
//	VkShaderModuleCreateInfo shaderModuleCreateInfo{};
//	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//	//shaderModuleCreateInfo.pNext;
//	//shaderModuleCreateInfo.flags;
//	shaderModuleCreateInfo.codeSize = code.size();
//	shaderModuleCreateInfo.pCode = code.data();
//
//	VK_CHECK(vkCreateShaderModule(Core.device, &shaderModuleCreateInfo, nullptr, &shaderModule));
//
//	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	//shaderStageCreateInfo.pNext;
//	//shaderStageCreateInfo.flags;
//	shaderStageCreateInfo.stage;
//	shaderStageCreateInfo.module = shaderModule;
//	shaderStageCreateInfo.pName = "main";
//	//shaderStageCreateInfo.pSpecializationInfo;
//}
//
////static void createRayTracingPipeline() {
////	VkShaderModuleCreateInfo ci{};
////
////	std::array<VkShaderStageFlagBits, 4> shaderStageFlagBits{
////		VK_SHADER_STAGE_RAYGEN_BIT_KHR, VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
////		VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, VK_SHADER_STAGE_MISS_BIT_KHR
////	};
////	std::array<const char*, 4> shaderCorePaths{
////	// insert paths to spv codes
////	};
////	std::array<VkShaderModule, 4> shaderModules{};
////	std::array<VkPipelineShaderStageCreateInfo, 4> shaderStages{};
////	for (size_t i = 0; i < 4; i++) {
////		getPipelineShaderStageCreateInfo(
////			shaderStageFlagBits[i],
////			readFileBytes(shaderCorePaths[i]),
////			shaderModules[i],
////			shaderStages[i]
////		);
////	}
////
////	VkRayTracingShaderGroupCreateInfoKHR shaderGroups{};
////
////	VkPipelineLayoutCreateInfo layoutCreateInfo{};
////	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
////	//layoutCreateInfo.pNext;
////	//layoutCreateInfo.flags;
////	//layoutCreateInfo.setLayoutCount;
////	//layoutCreateInfo.pSetLayouts;
////	//layoutCreateInfo.pushConstantRangeCount;
////	//layoutCreateInfo.pPushConstantRanges;
////
////	VK_CHECK(vkCreatePipelineLayout(Core.device, &layoutCreateInfo, nullptr, &Core.rayTracingPipeline.layout));
////
////	VkRayTracingPipelineCreateInfoKHR createInfo{};
////	createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
////	//createInfo.pNext;
////	//createInfo.flags;
////	createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
////	createInfo.pStages = shaderStages.data();
////	createInfo.groupCount = 1;
////	createInfo.pGroups = &shaderGroups;
////	createInfo.maxPipelineRayRecursionDepth = 10;
////	//createInfo.pLibraryInfo;
////	//createInfo.pLibraryInterface;
////	//createInfo.pDynamicState;
////	createInfo.layout = Core.rayTracingPipeline.layout;
////	//createInfo.basePipelineHandle;
////	//createInfo.basePipelineIndex;
////
////	VK_CHECK(vkCreateRayTracingPipelinesKHR(Core.device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &Core.rayTracingPipeline.handle));
////
////	for (size_t i = 0; i < shaderModules.size(); i++) {
////		vkDestroyShaderModule(Core.device, shaderModules[i], nullptr);
////	}
////}
////
////static void rtxTutorialStuff() {
////	VkAccelerationStructureCreateInfoKHR accelStructCreateInfo{};
////	accelStructCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
////	accelStructCreateInfo.pNext = nullptr;
////	accelStructCreateInfo.createFlags;
////	accelStructCreateInfo.buffer;
////	accelStructCreateInfo.offset;
////	accelStructCreateInfo.size;
////	accelStructCreateInfo.type;
////	accelStructCreateInfo.deviceAddress;
////
////	VkAccelerationStructureKHR accelStruct{ VK_NULL_HANDLE };
////	VK_CHECK(vkCreateAccelerationStructureKHR(Core.device, &accelStructCreateInfo, nullptr, &accelStruct));
////
////	VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
////	addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
////	addressInfo.pNext = nullptr;
////	addressInfo.accelerationStructure = accelStruct;
////
////	// --------------------
////	VkAccelerationStructureGeometryKHR triangleGeometry{};
////	triangleGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
////	//triangleGeometry.pNext;
////	triangleGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
////	triangleGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
////	//triangleGeometry.geometry.triangles.pNext;
////	triangleGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
////	triangleGeometry.geometry.triangles.vertexData;
////	triangleGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
////	triangleGeometry.geometry.triangles.maxVertex = 100;
////	triangleGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
////	triangleGeometry.geometry.triangles.indexData;
////	triangleGeometry.geometry.triangles.transformData;
////	triangleGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
////
////	VkAccelerationStructureBuildGeometryInfoKHR geometryInfos{};
////	geometryInfos.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
////	//geometryInfos.pNext;
////	geometryInfos.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
////	geometryInfos.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
////	geometryInfos.mode;
////	geometryInfos.srcAccelerationStructure;
////	geometryInfos.dstAccelerationStructure;
////	geometryInfos.geometryCount = 1;
////	geometryInfos.pGeometries = &triangleGeometry;
////	geometryInfos.ppGeometries;
////	geometryInfos.scratchData;
////
////	// --------------------
////
////	uint64_t blasAddress = vkGetAccelerationStructureDeviceAddressKHR(Core.device, &addressInfo);
////
////	VkTransformMatrixKHR instanceTransform = {
////		1.0f, 0.0f, 0.0f, 0.0f,
////		0.0f, 1.0f, 0.0f, 0.0f,
////		0.0f, 0.0f, 1.0f, 0.0f,
////	};
////
////	VkAccelerationStructureInstanceKHR instance{};
////	instance.transform = instanceTransform;
////	instance.instanceCustomIndex = 0;
////	instance.mask = 0xFF;
////	instance.instanceShaderBindingTableRecordOffset = 0;
////	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
////	instance.accelerationStructureReference;
////
////	// -----------------------
////
////	VkStridedDeviceAddressRegionKHR rayGenOffsets{};
////	rayGenOffsets.deviceAddress = 0;
////	rayGenOffsets.stride = Core.RTXPipelineProperties.maxShaderGroupStride;
////	//rayGenOffsets.size = rayGenOffsets.stride * numberOfRayGenShaders;
////
////	//vkCmdTraceRaysKHR();
////}
//
//static Buffer createBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
//	VkBufferCreateInfo createInfo{};
//	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//	createInfo.pNext = nullptr;
//	createInfo.flags = 0;
//	createInfo.size = allocSize;
//	createInfo.usage = usage;
//	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//	createInfo.queueFamilyIndexCount = 0;
//	createInfo.pQueueFamilyIndices = nullptr;
//
//	VmaAllocationCreateInfo vmaInfo{};
//	vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
//	vmaInfo.usage = memoryUsage;
//	vmaInfo.requiredFlags = 0;
//	vmaInfo.preferredFlags = 0;
//	vmaInfo.memoryTypeBits = 0;
//	vmaInfo.pool = VK_NULL_HANDLE;
//	vmaInfo.pUserData = nullptr;
//	vmaInfo.priority = 0;
//
//	Buffer buffer{};
//	VK_CHECK(vmaCreateBuffer(Core.allocator, &createInfo, &vmaInfo, &buffer.handle, &buffer.allocation, &buffer.info));
//
//	return buffer;
//}
//
//static BLAS createBLAS(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
//	BLAS accelStruct{};
//
//	accelStruct.vertices = createBuffer(vertices.size() * sizeof(Vertex),
//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
//		VMA_MEMORY_USAGE_GPU_ONLY);
//
//	accelStruct.indices = createBuffer(indices.size() * sizeof(uint32_t),
//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
//		VMA_MEMORY_USAGE_GPU_ONLY);
//
//	VkBufferDeviceAddressInfo verticesBufferAddressInfo{};
//	verticesBufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
//	//verticesBufferAddressInfo.pNext;
//	verticesBufferAddressInfo.buffer = accelStruct.vertices.handle;
//
//	VkBufferDeviceAddressInfo indicesBufferAddressInfo{};
//	indicesBufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
//	//indicesBufferAddressInfo.pNext;
//	indicesBufferAddressInfo.buffer = accelStruct.indices.handle;
//
//	accelStruct.geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
//	//accelStruct.geometry.pNext;
//	accelStruct.geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
//	accelStruct.geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
//	//accelStruct.geometry.geometry.triangles.pNext;
//	accelStruct.geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
//	accelStruct.geometry.geometry.triangles.vertexData.deviceAddress = vkGetBufferDeviceAddress(Core.device, &verticesBufferAddressInfo);
//	accelStruct.geometry.geometry.triangles.vertexStride = sizeof(Vertex);
//	accelStruct.geometry.geometry.triangles.maxVertex = vertices.size() - 1;
//	accelStruct.geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
//	accelStruct.geometry.geometry.triangles.indexData.deviceAddress = vkGetBufferDeviceAddress(Core.device, &indicesBufferAddressInfo);
//	//accelStruct.geometry.geometry.triangles.transformData;
//	accelStruct.geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
//
//	VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{};
//	buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
//	//buildGeometryInfo.pNext;
//	buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
//	buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
//	//buildGeometryInfo.mode;
//	//buildGeometryInfo.srcAccelerationStructure;
//	//buildGeometryInfo.dstAccelerationStructure;
//	buildGeometryInfo.geometryCount = 1;
//	buildGeometryInfo.pGeometries = &accelStruct.geometry;
//	//buildGeometryInfo.ppGeometries;
//	//buildGeometryInfo.scratchData;
//
//	VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo{};
//	buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
//	vkGetAccelerationStructureBuildSizesKHR(Core.device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
//		&buildGeometryInfo, &buildGeometryInfo.geometryCount, &buildSizesInfo);
//
//	accelStruct.scratchData = createBuffer(buildSizesInfo.buildScratchSize,
//		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
//
//	accelStruct.memory = createBuffer(buildSizesInfo.accelerationStructureSize,
//		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);
//
//	VkAccelerationStructureCreateInfoKHR accelStructCreateInfo{};
//	accelStructCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
//	//accelStructCreateInfo.pNext;
//	//accelStructCreateInfo.createFlags;
//	accelStructCreateInfo.buffer = accelStruct.memory.handle;
//	//accelStructCreateInfo.offset;
//	accelStructCreateInfo.size = buildSizesInfo.accelerationStructureSize;
//	accelStructCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
//	//accelStructCreateInfo.deviceAddress;
//
//	vkCreateAccelerationStructureKHR(Core.device, &accelStructCreateInfo, nullptr, &accelStruct.handle);
//
//	return accelStruct;
//}
//
//
//static TLAS createTLAS(std::vector<VkAccelerationStructureInstanceKHR> instances) {
//	TLAS accelStruct{};
//
//	accelStruct.geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
//	//accelStruct.geometry.pNext;
//	accelStruct.geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
//	accelStruct.geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
//	//accelStruct.geometry.geometry.instances.pNext;
//	accelStruct.geometry.geometry.instances.arrayOfPointers = VK_TRUE;
//	accelStruct.geometry.geometry.instances.data.hostAddress = instances.data(); // TODO: put data on device memory with buffers
//	accelStruct.geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
//
//	VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{};
//	buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
//	//buildGeometryInfo.pNext;
//	buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
//	buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
//	//buildGeometryInfo.mode;
//	//buildGeometryInfo.srcAccelerationStructure;
//	//buildGeometryInfo.dstAccelerationStructure;
//	buildGeometryInfo.geometryCount = 1;
//	buildGeometryInfo.pGeometries = &accelStruct.geometry;
//	//buildGeometryInfo.ppGeometries;
//	//buildGeometryInfo.scratchData;
//
//	VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo{};
//	buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
//	vkGetAccelerationStructureBuildSizesKHR(Core.device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
//		&buildGeometryInfo, &buildGeometryInfo.geometryCount, &buildSizesInfo);
//
//	accelStruct.scratchData = createBuffer(buildSizesInfo.buildScratchSize,
//		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
//
//	accelStruct.memory = createBuffer(buildSizesInfo.accelerationStructureSize,
//		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, VMA_MEMORY_USAGE_GPU_ONLY);
//
//	VkAccelerationStructureCreateInfoKHR accelStructCreateInfo{};
//	accelStructCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
//	//accelStructCreateInfo.pNext;
//	//accelStructCreateInfo.createFlags;
//	accelStructCreateInfo.buffer = accelStruct.memory.handle;
//	//accelStructCreateInfo.offset;
//	accelStructCreateInfo.size = buildSizesInfo.accelerationStructureSize;
//	accelStructCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
//	//accelStructCreateInfo.deviceAddress;
//
//	vkCreateAccelerationStructureKHR(Core.device, &accelStructCreateInfo, nullptr, &accelStruct.handle);
//
//	return accelStruct;
//}
//
void initGraphics(GraphicsInitInfo initInfo) {
  core = new Core(initInfo);

  // --- TEMPORARY CODE ---

  //std::vector<Vertex> vertices(3);
  //vertices[0].position = { 0.25f, 0.25f, 0.0f };
  //vertices[1].position = { 0.75f, 0.25f, 0.0f };
  //vertices[2].position = { 0.50f, 0.75f, 0.0f };

  //std::vector<uint32_t> indices = { 0, 1, 2 };
  //Core.triangleBLAS = createBLAS(vertices, indices);

  //immediateSubmit([&](VkCommandBuffer cmd) {
  //	const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &Core.triangleBLAS.getBuildRangeInfo();
  //	const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
  //	vkCmdBuildAccelerationStructuresKHR(cmd, 1, &Core.triangleBLAS.getBuildGeometryInfo(), cpcvkp);
  //	});

  //// --- TLAS BUILDING ---

  //VkTransformMatrixKHR instanceTransform = {
  //	1.0f, 0.0f, 0.0f, 0.0f,
  //	0.0f, 1.0f, 0.0f, 0.0f,
  //	0.0f, 0.0f, 1.0f, 0.0f,
  //};

  //VkAccelerationStructureInstanceKHR instance{};
  //instance.transform = instanceTransform;
  //instance.instanceCustomIndex = 0;
  //instance.mask = 0xFF;
  //instance.instanceShaderBindingTableRecordOffset = 0;
  //instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
  //instance.accelerationStructureReference = Core.triangleBLAS.getAddress();

  //Core.TLAS = createTLAS({ instance });

  //immediateSubmit([&](VkCommandBuffer cmd) {
  //	const VkAccelerationStructureBuildRangeInfoKHR* cvkp = &Core.TLAS.getBuildRangeInfo();
  //	const VkAccelerationStructureBuildRangeInfoKHR* const* cpcvkp = &cvkp;
  //	vkCmdBuildAccelerationStructuresKHR(cmd, 1, &Core.TLAS.getBuildGeometryInfo(), cpcvkp);
  //	});

  //Shader rayGenShader = Shader("resources/shaders/ray_gen.bin", VK_SHADER_STAGE_RAYGEN_BIT_KHR);
  //Shader rayChitShader = Shader("resources/shaders/ray_chit.bin", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
  //Shader rayMissShader = Shader("resources/shaders/ray_miss.bin", VK_SHADER_STAGE_MISS_BIT_KHR);

  //std::vector<VkPipelineShaderStageCreateInfo> shaderStages{
  //	rayGenShader.getStageCreateInfo(),
  //	rayChitShader.getStageCreateInfo(),
  //	rayMissShader.getStageCreateInfo()
  //};

  ////VkRayTracingShaderGroupCreateInfoKHR d;
  //std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups(3);
  //shaderGroups[0].sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
  //shaderGroups[0].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
  //shaderGroups[0].generalShader = 0;

  //shaderGroups[1].sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
  //shaderGroups[1].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
  //shaderGroups[1].closestHitShader = 1;

  //shaderGroups[2].sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
  //shaderGroups[2].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
  //shaderGroups[2].generalShader = 2;

  //std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
  //layoutBindings[0].binding = 0;
  //layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  //layoutBindings[0].descriptorCount = 1;
  //layoutBindings[0].stageFlags = VK_SHADER_STAGE_ALL;
  ////layoutBindings[0].pImmutableSamplers;

  //layoutBindings[1].binding = 1;
  //layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  //layoutBindings[1].descriptorCount = 1;
  //layoutBindings[1].stageFlags = VK_SHADER_STAGE_ALL;
  ////layoutBindings[1].pImmutableSamplers;

  //VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  //descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  ////descriptorSetLayoutCreateInfo.pNext;
  ////descriptorSetLayoutCreateInfo.flags;
  //descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
  //descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

  //VkDescriptorSetLayout descriptorSetLayout;
  //vkCreateDescriptorSetLayout(Core.device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

  //VkPipelineLayoutCreateInfo layoutCreateInfo{};
  //layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  ////layoutCreateInfo.pNext;
  ////layoutCreateInfo.flags;
  //layoutCreateInfo.setLayoutCount = 1;
  //layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
  ////layoutCreateInfo.pushConstantRangeCount;
  ////layoutCreateInfo.pPushConstantRanges;

  //vkCreatePipelineLayout(Core.device, &layoutCreateInfo, nullptr, &Core.rayTracingPipeline.layout);

  //VkRayTracingPipelineCreateInfoKHR createInfo{};
  //createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
  ////createInfo.pNext;
  ////createInfo.flags;
  //createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
  //createInfo.pStages = shaderStages.data();
  //createInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
  //createInfo.pGroups = shaderGroups.data();
  //createInfo.maxPipelineRayRecursionDepth = Core.RTPipelineProperties.maxRayRecursionDepth;
  ////createInfo.pLibraryInfo;
  ////createInfo.pLibraryInterface;
  ////createInfo.pDynamicState;
  //createInfo.layout = Core.rayTracingPipeline.layout;
  ////createInfo.basePipelineHandle;
  ////createInfo.basePipelineIndex;

  //vkCreateRayTracingPipelinesKHR(Core.device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &Core.rayTracingPipeline.handle);

  //vkGetRayTracingShaderGroupHandlesKHR()
}

void terminateGraphics() {
  delete core;
}

////void recordRenderer(MeshRenderer* renderer) {
////	GPUBuffers buffers{};
////	buffers.vertices = createBuffer(renderer->mesh.vertices.size() * sizeof(Vertex),
////		)
////}
//
//static VkCommandBufferBeginInfo getCommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0) {
//	VkCommandBufferBeginInfo beginInfo{};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	//beginInfo.pNext = nullptr;
//	beginInfo.flags = flags;
//	//beginInfo.pInheritanceInfo = nullptr;
//
//	return beginInfo;
//}
//
//static VkCommandBufferSubmitInfo getCommandBufferSubmitInfo(VkCommandBuffer commandBuffer) {
//	VkCommandBufferSubmitInfo submitInfo{};
//	submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
//	//submitInfo.pNext = nullptr;
//	submitInfo.commandBuffer = commandBuffer;
//	//submitInfo.deviceMask = 0;
//
//	return submitInfo;
//}
//
//static VkRenderingAttachmentInfo getRenderingAttachmentInfo(VkImageView view, VkClearValue* clear, VkImageLayout layout) {
//	VkRenderingAttachmentInfo colorAttachment{};
//	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
//	//colorAttachment.pNext = nullptr;
//
//	colorAttachment.imageView = view;
//	colorAttachment.imageLayout = layout;
//
//	colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
//	colorAttachment.resolveImageView = VK_NULL_HANDLE;
//	colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//
//	colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
//	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//	if (clear) {
//		colorAttachment.clearValue = *clear;
//	}
//
//	return colorAttachment;
//}
//
//static VkRenderingInfo getRenderingInfo(VkExtent2D extent, VkRenderingAttachmentInfo* renderAttachment) {
//	VkRenderingInfo renderInfo{};
//	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
//	//renderInfo.pNext = nullptr;
//	//renderInfo.flags = 0;
//	renderInfo.renderArea.extent = extent;
//	renderInfo.renderArea.offset = { 0, 0 };
//	renderInfo.layerCount = 1;
//	//renderInfo.viewMask = 0;
//	renderInfo.colorAttachmentCount = 1;
//	renderInfo.pColorAttachments = renderAttachment;
//	//renderInfo.pDepthAttachment = nullptr;
//	//renderInfo.pStencilAttachment = nullptr;
//
//	return renderInfo;
//}
//
//static VkSemaphoreSubmitInfo getSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
//	VkSemaphoreSubmitInfo submitInfo{};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
//	//submitInfo.pNext = nullptr;
//	submitInfo.semaphore = semaphore;
//	submitInfo.value = 1;
//	submitInfo.stageMask = stageMask;
//	//submitInfo.deviceIndex = 0;
//
//	return submitInfo;
//}
//
//static VkSubmitInfo2 getSubmitInfo(VkCommandBufferSubmitInfo* commandInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo) {
//	VkSubmitInfo2 info{};
//	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
//	//info.pNext = nullptr;
//	//info.flags = 0;
//
//	info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
//	info.pWaitSemaphoreInfos = waitSemaphoreInfo;
//
//	info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
//	info.pSignalSemaphoreInfos = signalSemaphoreInfo;
//
//	info.commandBufferInfoCount = 1;
//	info.pCommandBufferInfos = commandInfo;
//
//	return info;
//}
//
//static void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function) {
//	VK_CHECK(vkResetFences(Core.device, 1, &Core.ImmediateSubmit.fence));
//	VK_CHECK(vkResetCommandBuffer(Core.ImmediateSubmit.commandBuffer, 0));
//
//	VkCommandBuffer cmd = Core.ImmediateSubmit.commandBuffer;
//
//	VkCommandBufferBeginInfo cmdBeginInfo = getCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
//
//	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
//
//	function(cmd);
//
//	VK_CHECK(vkEndCommandBuffer(cmd));
//
//	VkCommandBufferSubmitInfo cmdinfo = getCommandBufferSubmitInfo(cmd);
//	VkSubmitInfo2 submit = getSubmitInfo(&cmdinfo, nullptr, nullptr);
//
//	// submit command buffer to the queue and execute it.
//	//  _renderFence will now block until the graphic commands finish execution
//	VK_CHECK(vkQueueSubmit2(Core.ImmediateSubmit.queue, 1, &submit, Core.ImmediateSubmit.fence));
//
//	VK_CHECK(vkWaitForFences(Core.device, 1, &Core.ImmediateSubmit.fence, true, 9999999999));
//}
//
//static void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout) {
//	VkImageMemoryBarrier2 imageBarrier{};
//	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
//	//imageBarrier.pNext = nullptr;
//
//	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
//	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
//	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
//	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
//
//	imageBarrier.oldLayout = currentLayout;
//	imageBarrier.newLayout = newLayout;
//
//	//imageBarrier.srcQueueFamilyIndex = 0;
//	//imageBarrier.dstQueueFamilyIndex = 0;
//
//	imageBarrier.image = image;
//	imageBarrier.subresourceRange.aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
//	//imageBarrier.subresourceRange.baseMipLevel = 0;
//	imageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
//	//imageBarrier.subresourceRange.baseArrayLayer = 0;
//	imageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
//
//	VkDependencyInfo depInfo{};
//	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
//	//depInfo.pNext = nullptr;
//	//depInfo.dependencyFlags = 0;
//
//	//depInfo.memoryBarrierCount = 0;
//	//depInfo.pBufferMemoryBarriers = nullptr;
//
//	//depInfo.bufferMemoryBarrierCount = 0;
//	//depInfo.pBufferMemoryBarriers = nullptr;
//
//	depInfo.imageMemoryBarrierCount = 1;
//	depInfo.pImageMemoryBarriers = &imageBarrier;
//
//	vkCmdPipelineBarrier2(cmd, &depInfo);
//}
//
//static void copyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize) {
//	VkImageBlit2 blitRegion{};
//	blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
//	//blitRegion.pNext = nullptr;
//
//	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	//blitRegion.srcSubresource.baseArrayLayer = 0;
//	blitRegion.srcSubresource.layerCount = 1;
//	//blitRegion.srcSubresource.mipLevel = 0;
//
//	//blitRegion.srcOffsets[0].x = 0;
//	//blitRegion.srcOffsets[0].y = 0;
//	//blitRegion.srcOffsets[0].z = 0;
//	blitRegion.srcOffsets[1].x = srcSize.width;
//	blitRegion.srcOffsets[1].y = srcSize.height;
//	blitRegion.srcOffsets[1].z = 1;
//
//	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	//blitRegion.dstSubresource.baseArrayLayer = 0;
//	blitRegion.dstSubresource.layerCount = 1;
//	//blitRegion.dstSubresource.mipLevel = 0;
//
//	//blitRegion.dstOffsets[0].x = 0;
//	//blitRegion.dstOffsets[0].y = 0;
//	//blitRegion.dstOffsets[0].z = 0;
//	blitRegion.dstOffsets[1].x = dstSize.width;
//	blitRegion.dstOffsets[1].y = dstSize.height;
//	blitRegion.dstOffsets[1].z = 1;
//
//	VkBlitImageInfo2 blitInfo{};
//	blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
//	//blitInfo.pNext = nullptr;
//	blitInfo.srcImage = source;
//	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//	blitInfo.dstImage = destination;
//	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//	blitInfo.regionCount = 1;
//	blitInfo.pRegions = &blitRegion;
//	blitInfo.filter = VK_FILTER_LINEAR;
//
//	vkCmdBlitImage2(commandBuffer, &blitInfo);
//}
//
//void render() {
//	// wait for gpu to finish rendering the last frame
//	VK_CHECK(vkWaitForFences(Core.device, 1, &Core.getCurrentFrame().renderFence, true, 1000000000));
//	VK_CHECK(vkResetFences(Core.device, 1, &Core.getCurrentFrame().renderFence));
//
//	// reset command buffer
//	VkCommandBuffer cmd = Core.getCurrentFrame().commandBuffer;
//	VK_CHECK(vkResetCommandBuffer(cmd, 0));
//	// begin recording command buffer
//	VkCommandBufferBeginInfo cmdBeginInfo = getCommandBufferBeginInfo();
//	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
//
//	// transition image into writeable mode before rendering
//	transitionImage(cmd, Core.drawImage.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
//
//	// begin a render pass connected to our draw image
//	VkRenderingAttachmentInfo colorAttachment = getRenderingAttachmentInfo(Core.drawImage.view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
//	VkRenderingInfo renderInfo = getRenderingInfo(Core.drawImage.extent, &colorAttachment);
//	vkCmdBeginRendering(cmd, &renderInfo);
//
//	// --- RENDER STUFF ---
//
//	vkCmdTraceRaysKHR();
//
//	// --- ---
//
//	vkCmdEndRendering(cmd);
//
//	// request an image from the swapchain
//	uint32_t swapchainImageIndex;
//	VK_CHECK(vkAcquireNextImageKHR(Core.device, Core.swapchain.handle, 1000000000, Core.getCurrentFrame().swapchainSemaphore, nullptr, &swapchainImageIndex));
//
//	// transition the draw image and the swapchain image into their correct transfer layouts
//	transitionImage(cmd, Core.drawImage.handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
//	transitionImage(cmd, Core.swapchain.images[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//
//	// copy drawimage to swapchain image
//	copyImageToImage(cmd, Core.drawImage.handle, Core.swapchain.images[swapchainImageIndex], Core.drawImage.extent, Core.swapchain.extent);
//
//	// make the swapchain image into presentable mode
//	transitionImage(cmd, Core.swapchain.images[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
//
//	// end the command buffer
//	VK_CHECK(vkEndCommandBuffer(cmd));
//
//	// prepare the submission queue
//	VkCommandBufferSubmitInfo cmdInfo = getCommandBufferSubmitInfo(cmd);
//	VkSemaphoreSubmitInfo waitInfo = getSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, Core.getCurrentFrame().swapchainSemaphore);
//	VkSemaphoreSubmitInfo signalInfo = getSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, Core.getCurrentFrame().renderSemaphore);
//	VkSubmitInfo2 submitInfo = getSubmitInfo(&cmdInfo, &signalInfo, &waitInfo);
//
//	// submit command buffer to queue and execute it
//	VK_CHECK(vkQueueSubmit2(Core.graphicsQueue, 1, &submitInfo, Core.getCurrentFrame().renderFence));
//
//	// prepare to present image to screen
//	VkPresentInfoKHR presentInfo{};
//	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//	presentInfo.pNext = nullptr;
//
//	presentInfo.pWaitSemaphores = &Core.getCurrentFrame().renderSemaphore;
//	presentInfo.waitSemaphoreCount = 1;
//
//	presentInfo.pSwapchains = &Core.swapchain.handle;
//	presentInfo.swapchainCount = 1;
//
//	presentInfo.pImageIndices = &swapchainImageIndex;
//
//	presentInfo.pResults = nullptr;
//
//	VK_CHECK(vkQueuePresentKHR(Core.graphicsQueue, &presentInfo));
//
//	//increase the number of frames drawn
//	Core.frameNumber++;
//}
