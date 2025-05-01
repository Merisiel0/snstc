#include "VulkanHandler.h"

#include "Allocator.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "DebugUtilsMessenger.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "ECS/ECS.h"
#include "Fence.h"
#include "Frame.h"
#include "GraphicsPipeline.h"
#include "Image.h"
#include "ImmediateSubmit.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Queue.h"
#include "Sampler.h"
#include "Semaphore.h"
#include "Shader.h"
#include "Swapchain.h"
#include "VulkanUtils.h"
#include "graphics/Window.h"
#include "resources/Material.h"
#include "resources/ResourceManager.h"

#include <array>
#include <optional>
#include <vector>

void VulkanHandler::beginDrawing(World& world) {
  std::shared_ptr<Frame> currentFrame = _swapchain->getCurrentFrame();

  // wait for gpu to finish rendering the last frame
  currentFrame->renderFence->wait();
  currentFrame->renderFence->reset();

  // reset command buffer
  currentFrame->commandBuffer->reset();
  // begin recording command buffer
  currentFrame->commandBuffer->begin();

  // update all instance buffers in MeshRenderers

  auto renderers = world.getComponentsInChildren<MeshRenderer>();
  for(const auto& renderer : renderers) {
    MeshRenderer& rd = renderer.get();
    rd.updateInstanceBuffer(currentFrame->commandBuffer);
  }

  // clear image with color - todo: insert skybox here
  _drawImage->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  _drawImage->clear(currentFrame->commandBuffer, {1, 1, 1, 1});

  // transition image into writeable mode before rendering
  _drawImage->transitionLayout(currentFrame->commandBuffer,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  // update camera information
  world.updateCamera(currentFrame->commandBuffer);

  // begin a render pass connected to our draw image
  currentFrame->commandBuffer->beginRendering(*_drawImage, *_depthImage);

  // set constant dynamic states
  VkViewport viewport {};
  viewport.x = (float) _window->position.x;
  viewport.y = (float) _window->position.y;
  viewport.width = (float) _window->extent.x;
  viewport.height = (float) _window->extent.y;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor {};
  scissor.offset.x = _window->position.x >= 0 ? (int32_t) _window->position.x : 0;
  scissor.offset.y = _window->position.y >= 0 ? (int32_t) _window->position.y : 0;
  scissor.extent.width = _window->extent.x;
  scissor.extent.height = _window->extent.y;

  currentFrame->commandBuffer->setViewport(&viewport);
  currentFrame->commandBuffer->setScissor(&scissor);

  // write camera descriptor set
  currentFrame->sceneDescSet->write(0, *world.camBuffer);
  currentFrame->sceneDescSet->write(1, *world.lightsBuffer);
  currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->sceneDescSet, 0,
    *_graphicsPipelines[0]);
}

void VulkanHandler::endDrawing() {
  std::shared_ptr<Frame> currentFrame = _swapchain->getCurrentFrame();

  currentFrame->commandBuffer->endRendering();

  // request an image from the swapchain
  uint32_t swapchainImageIndex = _swapchain->acquireNextImage();

  // transition the draw image and the swapchain image into their correct
  // transfer layouts
  _drawImage->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  _swapchain->getImage(swapchainImageIndex)
    ->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  // copy drawimage to swapchain image
  _drawImage->blitTo(currentFrame->commandBuffer, _swapchain->getImage(swapchainImageIndex));

  // make the swapchain image into presentable mode
  _swapchain->getImage(swapchainImageIndex)
    ->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

  // end the command buffer
  currentFrame->commandBuffer->end();

  // submit command buffer to queue and execute it
  currentFrame->commandBuffer->submitToQueue(*_device->graphicsQueue, *currentFrame->renderFence,
    currentFrame->swapchainSemaphore,
    currentFrame->renderSemaphore); // currentFrame->swapchainSemaphore

  // present image to screen
  _device->graphicsQueue->present(*_swapchain, swapchainImageIndex, *currentFrame->renderSemaphore);

  // increase the number of frames drawn
  _swapchain->frameNumber++;
}

VulkanHandler::VulkanHandler(const char* applicationName, int applicationVersion,
  const char* engineName, int engineVersion) {
  // instance
  std::vector<const char*> instanceLayers {
#ifdef VKDEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
  };

  std::vector<const char*> instanceExtensions {
#ifdef VKDEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};

  uint32_t sdlExtensionCount;
  const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
  SDL_CHECK(sdlExtensions);
  for(uint32_t i = 0; i < sdlExtensionCount; i++) {
    instanceExtensions.push_back(sdlExtensions[i]);
  }

  _instance = std::make_shared<Instance>(applicationName, applicationVersion, engineName,
    engineVersion, instanceLayers, instanceExtensions);

  fetchExtensionFunctionPointers(_instance->handle);

  // window
  _window = std::make_shared<Window>("SphereNSTC", _instance);
  _window->setIcon(ResourceManager::assetsPath + "/src/assets/sprites/icon.png");

  // debug messenger
#ifdef VKDEBUG
  _debugMessenger = std::make_shared<DebugUtilsMessenger>(_instance);
#endif

  // physical device, device and allocator
  std::vector<const char*> deviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  _physicalDevice = std::make_shared<PhysicalDevice>(_instance, deviceExtensions);
  _device = std::make_shared<Device>(_physicalDevice, deviceExtensions);
  _allocator = std::make_shared<Allocator>(_instance, _physicalDevice, _device);

  _immediateSubmit = std::make_shared<ImmediateSubmit>(_device);

  // init classes
  Buffer::init(_device, _allocator, _immediateSubmit);
  Image::init(_device, _allocator, _immediateSubmit);

  // descriptor pool
  std::vector<VkDescriptorPoolSize> poolSizes;
  VkDescriptorPoolSize poolSize {};
  poolSize.descriptorCount =
    Swapchain::FRAME_OVERLAP * 2; // times 2 for cam buffer and light buffer
  poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes.push_back(poolSize);

  poolSize.descriptorCount = Swapchain::FRAME_OVERLAP * MapIndex::MAP_COUNT;
  poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes.push_back(poolSize);

  _descriptorPool = std::make_shared<DescriptorPool>(_device, poolSizes);

  // descriptor set layouts
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  // camera descriptor set layout
  VkDescriptorSetLayoutBinding binding;
  binding.binding = 0; // camera buffer
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  binding.pImmutableSamplers = nullptr;
  bindings.push_back(binding);

  binding.binding = 1; // lights buffer
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  bindings.push_back(binding);

  _sceneDescSetLayout =
    std::make_shared<DescriptorSetLayout>(_device, bindings, DESCRIPTOR_SET_LAYOUT_SCENE);

  // skybox descriptor set layout
  bindings.clear();

  binding.binding = 0; // cubemap
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  bindings.push_back(binding);

  _skyboxDescSetLayout =
    std::make_shared<DescriptorSetLayout>(_device, bindings, DESCRIPTOR_SET_LAYOUT_SKYBOX);

  // object descriptor set layout
  bindings.clear();

  binding.binding = 0; // albedo texture
  bindings.push_back(binding);

  binding.binding = 1; // normal texture
  bindings.push_back(binding);

  binding.binding = 2; // rougness texture
  bindings.push_back(binding);

  binding.binding = 3; // metalness texture
  bindings.push_back(binding);

  binding.binding = 4; // height texture
  bindings.push_back(binding);

  binding.binding = 5; // ambiant occlusion texture
  bindings.push_back(binding);

  _objDescSetLayout =
    std::make_shared<DescriptorSetLayout>(_device, bindings, DESCRIPTOR_SET_LAYOUT_OBJECT);

  _swapchain = std::make_shared<Swapchain>(*_window, _device, *_descriptorPool,
    std::vector<std::shared_ptr<DescriptorSetLayout>> {_sceneDescSetLayout, _skyboxDescSetLayout,
      _objDescSetLayout});

  _drawImage = std::make_shared<Image>(*_swapchain, COLOR,
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

  _depthImage =
    std::make_shared<Image>(*_swapchain, DEPTH, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  _immediateSubmit->submit([&](std::shared_ptr<CommandBuffer> cmd) {
    _depthImage->transitionLayout(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  });

  _defaultSampler = std::make_shared<Sampler>(_device);

  // create pipelines
  _graphicsPipelines.resize(GRAPHICS_PIPELINE_ID_COUNT);

  // skybox pipeline
  Shader* vertexShader =
    new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/skybox_vert.spv",
      VK_SHADER_STAGE_VERTEX_BIT);
  Shader* fragmentShader =
    new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/skybox_frag.spv",
      VK_SHADER_STAGE_FRAGMENT_BIT);

  std::vector<VkPipelineShaderStageCreateInfo> shaderInfos = {vertexShader->getStageCreateInfo(),
    fragmentShader->getStageCreateInfo()};

  std::vector<VkDescriptorSetLayout> setLayouts = {_sceneDescSetLayout->handle,
    _skyboxDescSetLayout->handle};

  _graphicsPipelines[GRAPHICS_PIPELINE_SKYBOX] = std::make_shared<GraphicsPipeline>(_device,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, shaderInfos, setLayouts);

  delete vertexShader;
  delete fragmentShader;

  // try {
  //   // load shaders
  //   Shader* vertexShader =
  //     new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/PBR_material_vert.spv",
  //       VK_SHADER_STAGE_VERTEX_BIT);
  //   Shader* fragmentShader =
  //     new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/PBR_material_frag.spv",
  //       VK_SHADER_STAGE_FRAGMENT_BIT);

  //   std::vector<VkPipelineShaderStageCreateInfo> shaderInfos = {vertexShader->getStageCreateInfo(),
  //     fragmentShader->getStageCreateInfo()};

  //   std::vector<VkDescriptorSetLayout> setLayouts = {_sceneDescSetLayout->handle,
  //     _skyboxDescSetLayout->handle, _objDescSetLayout->handle};

  //   // create normal pbr pipeline
  //   _pipelinePBR = std::make_shared<GraphicsPipeline>(_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  //     VK_POLYGON_MODE_FILL, shaderInfos, setLayouts);

  //   // create line pbr pipeline
  //   _pipelineLinePBR = std::make_shared<GraphicsPipeline>(_device,
  //     VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE, shaderInfos, setLayouts);

  //   delete vertexShader;
  //   delete fragmentShader;
  // } catch(const std::runtime_error& e) {
  //   std::cerr << e.what() << std::endl;
  //   std::cerr << "Failed to instantiate PBR pipeline." << std::endl;
  // }
}

VulkanHandler::~VulkanHandler() {
  _device->waitIdle();

  for(int i = 0; i < GRAPHICS_PIPELINE_ID_COUNT; i++) {
    _graphicsPipelines[i].reset();
  }

  _defaultSampler.reset();

  _depthImage.reset();
  _drawImage.reset();
  _swapchain.reset();

  _sceneDescSetLayout.reset();
  _skyboxDescSetLayout.reset();
  _objDescSetLayout.reset();
  _descriptorPool.reset();

  _immediateSubmit.reset();

  _allocator.reset();
  _device.reset();

#ifdef VKDEBUG
  _debugMessenger.reset();
#endif // VKDEBUG

  _window.reset();
  _instance.reset();
}

void VulkanHandler::render(World& world) {
  // begin
  beginDrawing(world);

  std::shared_ptr<Frame> currentFrame = _swapchain->getCurrentFrame();

  // draw mesh renderers
  // auto renderers = world.getComponentsInChildren<MeshRenderer>();
  // for(const auto& renderer : renderers) {
  //   MeshRenderer& rd = renderer.get();

  //   // find and bind pipeline to use
  //   std::shared_ptr<GraphicsPipeline> currentPipeline = _pipelinePBR;
  //   currentFrame->commandBuffer->bindPipeline(*currentPipeline);

  //   // skipped object descriptor binding
  //   currentFrame->objDescSet->write(0, rd.material->getMap(ALBEDO), *_defaultSampler);
  //   currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->objDescSet, 1, *currentPipeline);

  //   // push vertices and model matrix inside pushconstants
  //   PushConstants constants {};
  //   constants.transform = rd.gameObject->getComponent<Transform>()->modelMatrix();
  //   constants.vertexBuffer = rd.mesh->vertices->address;
  //   std::shared_ptr<Buffer> instanceBuffer = rd.getInstancesBuffer();
  //   constants.instanceBuffer = instanceBuffer->address;
  //   currentFrame->commandBuffer->pushConstants(constants, currentPipeline->layout(),
  //     VK_SHADER_STAGE_VERTEX_BIT);

  //   // set dynamic states
  //   currentFrame->commandBuffer->setLineWidth(rd.lineWidth);
  //   currentFrame->commandBuffer->setCullMode(rd.cullMode);

  //   // draw
  //   currentFrame->commandBuffer->bindIndexBuffer(rd.mesh->indices);
  //   currentFrame->commandBuffer->drawIndexed(rd.mesh->indices->count(), rd.getInstanceCount());
  // }

  // draw skybox
  Skybox* skybox = world.getComponentInChildren<Skybox>();
  if(skybox) {
    // bind pipeline
    currentFrame->commandBuffer->bindPipeline(*_graphicsPipelines[GRAPHICS_PIPELINE_SKYBOX]);

    // write and bind descriptor sets
    Transform* skyboxTransform = skybox->gameObject->getComponent<Transform>();
    currentFrame->skyboxDescSet->write(0, *skybox->getCubeMap(), *_defaultSampler);
    currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->skyboxDescSet, 1,
      *_graphicsPipelines[GRAPHICS_PIPELINE_SKYBOX]);

    // push constants
    PushConstants constants;
    constants.model = skyboxTransform->getRotationMatrix(WORLD);
    constants.vertexBuffer = skybox->getMesh()->vertices->address;
    currentFrame->commandBuffer->pushConstants(constants,
      _graphicsPipelines[GRAPHICS_PIPELINE_SKYBOX]->layout(), VK_SHADER_STAGE_VERTEX_BIT);

    // set dynamic states
    currentFrame->commandBuffer->setCullMode(VK_CULL_MODE_NONE);

    // draw
    currentFrame->commandBuffer->drawVertices(skybox->getMesh()->vertices->count());
  }

  // end
  endDrawing();
}

void VulkanHandler::waitForEndOfWork() const { _device->waitIdle(); }