#include "VulkanHandler.h"

#include "Allocator.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "DebugUtilsMessenger.h"
#include "DescriptorManager.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "ECS/ECS.h"
#include "Fence.h"
#include "Frame.h"
#include "GraphicsPipeline.h"
#include "GraphicsPipelineId.h"
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
#include <unordered_map>
#include <vector>

std::shared_ptr<Instance> VulkanHandler::_instance;

#ifdef VKDEBUG
std::shared_ptr<DebugUtilsMessenger> VulkanHandler::_debugMessenger;
#endif // VKDEBUG

std::shared_ptr<PhysicalDevice> VulkanHandler::_physicalDevice;
std::shared_ptr<Device> VulkanHandler::_device;
std::shared_ptr<Allocator> VulkanHandler::_allocator;
std::shared_ptr<ImmediateSubmit> VulkanHandler::_immediateSubmit;
std::shared_ptr<Window> VulkanHandler::_window;
std::shared_ptr<Swapchain> VulkanHandler::_swapchain;
std::shared_ptr<Image> VulkanHandler::_drawImage;
std::shared_ptr<Image> VulkanHandler::_depthImage;
std::unordered_map<GraphicsPipelineId, std::shared_ptr<GraphicsPipeline>>
  VulkanHandler::_graphicsPipelines;
std::shared_ptr<Sampler> VulkanHandler::_defaultSampler;

void VulkanHandler::beginDrawing(World& world) {
  std::shared_ptr<Frame> currentFrame = _swapchain->getCurrentFrame();

  // wait for gpu to finish rendering the last frame
  currentFrame->renderFence->wait();
  currentFrame->renderFence->reset();

  // reset command buffer
  currentFrame->commandBuffer->reset();
  // begin recording command buffer
  currentFrame->commandBuffer->begin();

  // clear image with color
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
  currentFrame->globalDescSet->write(0, *world.camBuffer);
  currentFrame->globalDescSet->write(1, *world.lightsBuffer);
  currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->globalDescSet, 0,
    *_graphicsPipelines.begin()->second);
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
    currentFrame->swapchainSemaphore, currentFrame->renderSemaphore);

  // present image to screen
  _device->graphicsQueue->present(*_swapchain, swapchainImageIndex, *currentFrame->renderSemaphore);

  // increase the number of frames drawn
  _swapchain->frameNumber++;
}

void VulkanHandler::init(const char* applicationName, int applicationVersion,
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
  DescriptorManager::init(_device);

  _swapchain = std::make_shared<Swapchain>(*_window, _device);

  _drawImage = std::make_shared<Image>(*_swapchain, COLOR,
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

  _depthImage =
    std::make_shared<Image>(*_swapchain, DEPTH, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  _immediateSubmit->submit([&](std::shared_ptr<CommandBuffer> cmd) {
    _depthImage->transitionLayout(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  });

  _defaultSampler = std::make_shared<Sampler>(_device);

  // skybox pipeline
  Shader* skyboxVert =
    new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/skybox_vert.spv",
      VK_SHADER_STAGE_VERTEX_BIT);
  Shader* skyboxFrag =
    new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/skybox_frag.spv",
      VK_SHADER_STAGE_FRAGMENT_BIT);

  std::vector<VkPipelineShaderStageCreateInfo> shaderInfos = {skyboxVert->getStageCreateInfo(),
    skyboxFrag->getStageCreateInfo()};

  std::vector<VkDescriptorSetLayout> setLayouts = {
    DescriptorManager::getLayout(DESCRIPTOR_SET_LAYOUT_GLOBAL).getHandle(),
    DescriptorManager::getLayout(DESCRIPTOR_SET_LAYOUT_SKYBOX).getHandle()};

  GraphicsPipelineId pipelineId = GraphicsPipelineId(UNIQUE_GRAPHICS_PIPELINE_SKYBOX);

  std::pair<GraphicsPipelineId, std::shared_ptr<GraphicsPipeline>> mapPair = std::make_pair(
    pipelineId, std::make_shared<GraphicsPipeline>(_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                  VK_POLYGON_MODE_FILL, shaderInfos, setLayouts, false));

  _graphicsPipelines.insert(mapPair);

  delete skyboxVert;
  delete skyboxFrag;

  // load shaders
  Shader* staticmeshVert =
    new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/staticmesh_vert.spv",
      VK_SHADER_STAGE_VERTEX_BIT);

  Shader* unlitFrag =
    new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/unlit_frag.spv",
      VK_SHADER_STAGE_FRAGMENT_BIT);

  // unlit static meshes pipelines
  shaderInfos = {staticmeshVert->getStageCreateInfo(), unlitFrag->getStageCreateInfo()};

  setLayouts.pop_back();
  setLayouts.push_back(DescriptorManager::getLayout(DESCRIPTOR_SET_LAYOUT_MATERIAL).getHandle());

  pipelineId = GraphicsPipelineId(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
    MESH_LAYOUT_STATIC, LIGHTING_TYPE_UNLIT);

  mapPair = std::make_pair(pipelineId,
    std::make_shared<GraphicsPipeline>(_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      VK_POLYGON_MODE_FILL, shaderInfos, setLayouts));

  _graphicsPipelines.insert(mapPair);

  pipelineId = GraphicsPipelineId(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE,
    MESH_LAYOUT_STATIC, LIGHTING_TYPE_UNLIT);

  mapPair = std::make_pair(pipelineId,
    std::make_shared<GraphicsPipeline>(_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      VK_POLYGON_MODE_LINE, shaderInfos, setLayouts));

  _graphicsPipelines.insert(mapPair);

  // cleanup shaders
  delete staticmeshVert;
  delete unlitFrag;
}

void VulkanHandler::cleanup() {
  _device->waitIdle();

  _graphicsPipelines.clear();

  _defaultSampler.reset();

  _depthImage.reset();
  _drawImage.reset();
  _swapchain.reset();

  DescriptorManager::cleanup();

  _immediateSubmit.reset();

  _allocator.reset();
  _device.reset();

#ifdef VKDEBUG
  _debugMessenger.reset();
#endif // VKDEBUG

  _window.reset();
  _instance.reset();
}

void VulkanHandler::loadPipeline(GraphicsPipelineId id) {}

void VulkanHandler::releasePipeline(GraphicsPipelineId id) {}

void VulkanHandler::render(World& world) {
  // begin
  beginDrawing(world);

  std::shared_ptr<Frame> currentFrame = _swapchain->getCurrentFrame();

  // draw static meshes
  auto renderers = world.getComponentsInChildren<MeshRenderer>();
  for(const auto& renderer : renderers) {
    MeshRenderer& rd = renderer.get();

    // find and bind pipeline to use
    std::shared_ptr<GraphicsPipeline> currentPipeline =
      _graphicsPipelines.at(rd.getGraphicsPipelineId());
    currentFrame->commandBuffer->bindPipeline(*currentPipeline);

    // write and bind descriptor sets
    rd.material->updateDescriptorSet(_swapchain->getFrameIndex(), *_defaultSampler);
    currentFrame->commandBuffer->bindDescriptorSet(
      *rd.material->getDescriptorSet(_swapchain->getFrameIndex()), 1, *currentPipeline);

    // push vertices and model matrix inside pushconstants
    PushConstants constants {};
    constants.model = rd.gameObject->getComponent<Transform>()->getModelMatrix(WORLD);
    constants.vertexBuffer = rd.mesh->vertices->address;
    currentFrame->commandBuffer->pushConstants(constants, currentPipeline->layout(),
      VK_SHADER_STAGE_VERTEX_BIT);

    // set dynamic states
    currentFrame->commandBuffer->setLineWidth(rd.lineWidth);
    currentFrame->commandBuffer->setCullMode(rd.cullMode);

    // write and bind descriptor sets
    currentFrame->commandBuffer->bindIndexBuffer(rd.mesh->indices);
    currentFrame->commandBuffer->drawIndexed(rd.mesh->indices->count(), 1);
  }

  // draw skybox
  Skybox* skybox = world.getComponentInChildren<Skybox>();
  if(skybox) {
    // bind pipeline
    GraphicsPipelineId skyboxPipelineId = GraphicsPipelineId(UNIQUE_GRAPHICS_PIPELINE_SKYBOX);
    std::shared_ptr<GraphicsPipeline> currentPipeline = _graphicsPipelines.at(skyboxPipelineId);
    currentFrame->commandBuffer->bindPipeline(*currentPipeline);

    // write and bind descriptor sets
    Transform* skyboxTransform = skybox->gameObject->getComponent<Transform>();
    currentFrame->skyboxDescSet->write(0, *skybox->getCubeMap(), *_defaultSampler);
    currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->skyboxDescSet, 1,
      *currentPipeline);

    // push constants
    PushConstants constants;
    constants.model = skyboxTransform->getRotationMatrix(WORLD);
    constants.vertexBuffer = skybox->getMesh()->vertices->address;
    currentFrame->commandBuffer->pushConstants(constants, currentPipeline->layout(),
      VK_SHADER_STAGE_VERTEX_BIT);

    // set dynamic states
    currentFrame->commandBuffer->setCullMode(VK_CULL_MODE_NONE);

    // draw
    currentFrame->commandBuffer->drawVertices(skybox->getMesh()->vertices->count());
  }

  // end
  endDrawing();
}

void VulkanHandler::waitForEndOfWork() { _device->waitIdle(); }