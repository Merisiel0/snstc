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
  currentFrame->camDescSet->write(0, world.camBuffer);
  currentFrame->camDescSet->write(1, world.lightsBuffer);
  currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->camDescSet, 0, *_pipelineColor);
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

  // cam shader stage descriptor set layout
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  VkDescriptorSetLayoutBinding binding {};
  binding.binding = 0;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // camera buffer
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  bindings.push_back(binding);

  binding.binding = 1; // lights buffer
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  bindings.push_back(binding);

  _camDescSetLayout = std::make_shared<DescriptorSetLayout>(_device, bindings);

  bindings.clear();

  binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // color texture
  binding.binding = 0;
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

  _objDescSetLayout = std::make_shared<DescriptorSetLayout>(_device, bindings);

  _swapchain = std::make_shared<Swapchain>(*_window, _device, *_descriptorPool, *_camDescSetLayout,
    *_objDescSetLayout);

  _drawImage = std::make_shared<Image>(*_swapchain, COLOR,
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

  _depthImage =
    std::make_shared<Image>(*_swapchain, DEPTH, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  _immediateSubmit->submit([&](std::shared_ptr<CommandBuffer> cmd) {
    _depthImage->transitionLayout(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  });

  _defaultSampler = std::make_shared<Sampler>(_device);

  // PBR pipelines
  try {
    Shader* vertexShader = new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/PBR_material_vert.spv",
      VK_SHADER_STAGE_VERTEX_BIT);
    Shader* fragmentShader = new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/PBR_material_frag.spv",
      VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkPipelineShaderStageCreateInfo> shaderInfos = {vertexShader->getStageCreateInfo(),
      fragmentShader->getStageCreateInfo()};

    std::vector<VkDescriptorSetLayout> setLayouts = {_camDescSetLayout->handle,
      _objDescSetLayout->handle};

    // create normal pbr pipeline
    _pipelinePBR = std::make_shared<GraphicsPipeline>(_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      VK_POLYGON_MODE_FILL, shaderInfos, setLayouts);

    // create line pbr pipeline
    _pipelineLinePBR = std::make_shared<GraphicsPipeline>(_device,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE, shaderInfos, setLayouts);

    delete vertexShader;
    delete fragmentShader;
  } catch(const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "Failed to instantiate PBR pipeline." << std::endl;
  }

  // plain color pipelines
  try {
    Shader* vertexShader = new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/color_material_vert.spv",
      VK_SHADER_STAGE_VERTEX_BIT);
    Shader* fragmentShader = new Shader(_device, ResourceManager::assetsPath + "/src/assets/shaders/color_material_frag.spv",
      VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkPipelineShaderStageCreateInfo> shaderInfos = {vertexShader->getStageCreateInfo(),
      fragmentShader->getStageCreateInfo()};

    std::vector<VkDescriptorSetLayout> setLayouts = {_camDescSetLayout->handle};

    // create normal color pipeline
    _pipelineColor = std::make_shared<GraphicsPipeline>(_device,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, shaderInfos, setLayouts);

    // create line color pipeline
    _pipelineLineColor = std::make_shared<GraphicsPipeline>(_device,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE, shaderInfos, setLayouts);

    delete vertexShader;
    delete fragmentShader;

  } catch(const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "Failed to instantiate plain color pipeline." << std::endl;
  }
}

VulkanHandler::~VulkanHandler() {
  _device->waitIdle();

  _pipelineLineColor.reset();
  _pipelineColor.reset();
  _pipelineLinePBR.reset();
  _pipelinePBR.reset();

  _defaultSampler.reset();

  _depthImage.reset();
  _drawImage.reset();
  _swapchain.reset();

  _camDescSetLayout.reset();
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

  // for each renderers in the scene..
  auto renderers = world.getComponentsInChildren<MeshRenderer>();
  for(const auto& renderer : renderers) {
    MeshRenderer& rd = renderer.get();

    // choose which pipeline to use
    std::shared_ptr<GraphicsPipeline> currentPipeline;
    if(!rd.material->isPlainColor()) {
      if(rd.polygonMode == VK_POLYGON_MODE_LINE)
        currentPipeline = _pipelineLinePBR;
      else
        currentPipeline = _pipelinePBR;
    } else if(rd.polygonMode == VK_POLYGON_MODE_LINE) {
      currentPipeline = _pipelineLineColor;
    } else {
      currentPipeline = _pipelineColor;
    }
    currentFrame->commandBuffer->bindPipeline(*currentPipeline);

    // write object descriptor set if needed
    if(!rd.material->isPlainColor()) {
      if(rd.material->hasMap(ALBEDO)) {
        currentFrame->objDescSet->write(0, rd.material->getMap(ALBEDO), *_defaultSampler);
      }
      /*if (renderer->material->getNormal()) {
        currentFrame->objDescSet->write(1, renderer->material->getNormal(),
      _defaultSampler);
      }
      if (renderer->material->getRoughness()) {
        currentFrame->objDescSet->write(2, renderer->material->getRoughness(),
      _defaultSampler);
      }
      if (renderer->material->getMetalness()) {
        currentFrame->objDescSet->write(3, renderer->material->getMetalness(),
      _defaultSampler);
      }
      if (renderer->material->getHeight()) {
        currentFrame->objDescSet->write(4, renderer->material->getHeight(),
      _defaultSampler);
      }
      if (renderer->material->getAO()) {
        currentFrame->objDescSet->write(5, renderer->material->getAO(),
      _defaultSampler);
      }*/

      currentFrame->commandBuffer->bindDescriptorSet(*currentFrame->objDescSet, 1,
        *currentPipeline);
    }

    // push vertices and model matrix inside pushconstants
    PushConstants constants {};
    constants.transform = rd.gameObject->getComponent<Transform>()->modelMatrix();
    // constants.transform = world->camera()->projection * world->camera()->view
    // * renderer->gameObject->getComponent<Transform>()->modelMatrix();
    constants.vertexBuffer = rd.mesh->vertices->address;
    currentFrame->commandBuffer->pushConstants(constants, currentPipeline->layout(),
      VK_SHADER_STAGE_VERTEX_BIT);

    // set dynamic states
    currentFrame->commandBuffer->setLineWidth(rd.lineWidth);
    currentFrame->commandBuffer->setCullMode(rd.cullMode);

    // draw
    currentFrame->commandBuffer->bindIndexBuffer(rd.mesh->indices);
    currentFrame->commandBuffer->drawIndexed(rd.mesh->indices->count());
  }

  // end
  endDrawing();
}

void VulkanHandler::waitForEndOfWork() const { _device->waitIdle(); }
