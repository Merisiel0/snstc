#include "GraphicsHandler.h"

#include "VulkanUtils.h"

#include "ImmediateSubmit.h"
#include "DebugUtilsMessenger.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Queue.h"
#include "Allocator.h"
#include "Window.h"
#include "Swapchain.h"
#include "Frame.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Fence.h"
#include "Buffer.h"
#include "Image.h"
#include "Shader.h"
#include "GraphicsPipeline.h"
//#include "BLAS.h"
//#include "TLAS.h"

#include "_ECS/ECS.h";
#include "_resources/Material.h"

#include <vector>
#include <optional>
#include <array>

void GraphicsHandler::beginDrawing() {
  Frame* currentFrame = _swapchain->getCurrentFrame();

  // wait for gpu to finish rendering the last frame
  currentFrame->renderFence->wait();
  currentFrame->renderFence->reset();

  // reset command buffer
  currentFrame->commandBuffer->reset();
  // begin recording command buffer
  currentFrame->commandBuffer->begin();

  // transition image into writeable mode before rendering
  _drawImage->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  // begin a render pass connected to our draw image
  currentFrame->commandBuffer->beginRendering(_drawImage);
}

void GraphicsHandler::endDrawing() {
  Frame* currentFrame = _swapchain->getCurrentFrame();

  currentFrame->commandBuffer->endRendering();

  // request an image from the swapchain
  uint32_t swapchainImageIndex = _swapchain->acquireNextImage();

  // transition the draw image and the swapchain image into their correct transfer layouts
  _drawImage->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  _swapchain->getImage(swapchainImageIndex)->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  // copy drawimage to swapchain image
  _drawImage->copyTo(currentFrame->commandBuffer, _swapchain->getImage(swapchainImageIndex));

  // make the swapchain image into presentable mode
  _swapchain->getImage(swapchainImageIndex)->transitionLayout(currentFrame->commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

  // end the command buffer
  currentFrame->commandBuffer->end();

  // submit command buffer to queue and execute it
  currentFrame->commandBuffer->submitToQueue(_device->graphicsQueue, currentFrame->renderFence, currentFrame->swapchainSemaphore, currentFrame->renderSemaphore);

  // present image to screen
  _device->graphicsQueue->present(_swapchain, swapchainImageIndex, currentFrame->renderSemaphore);

  //increase the number of frames drawn
  _swapchain->frameNumber++;
}

GraphicsHandler::GraphicsHandler(GraphicsInitInfo initInfo) {
  _window = new Window("SphereNSTC");

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
  SDL_BCHECK(SDL_Vulkan_GetInstanceExtensions(_window->handle, &sdlExtensionCount, nullptr));
  std::vector<const char*> sdlExtensions(sdlExtensionCount);
  SDL_BCHECK(SDL_Vulkan_GetInstanceExtensions(_window->handle, &sdlExtensionCount, sdlExtensions.data()));
  for (uint32_t i = 0; i < sdlExtensionCount; i++) {
    instanceExtensions.push_back(sdlExtensions[i]);
  }

  _instance = new Instance(initInfo.applicationName, initInfo.applicationVersion, initInfo.engineName, initInfo.engineVersion, instanceLayers, instanceExtensions);

  fetchExtensionFunctionPointers(_instance->handle);

#ifdef VKDEBUG
  _debugMessenger = new DebugUtilsMessenger(_instance);
#endif // VKDEBUG

  std::vector<const char*> deviceExtensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
  };
  _physicalDevice = new PhysicalDevice(_instance, deviceExtensions);
  _device = new Device(_physicalDevice, deviceExtensions);
  _allocator = new Allocator(_instance, _physicalDevice, _device);

  Buffer::init(_device, _allocator, _immediateSubmit);
  Image::init(_device, _allocator, _immediateSubmit);

  _immediateSubmit = new ImmediateSubmit(_device);

  _swapchain = new Swapchain(_window, _instance, _physicalDevice, _device);

  _drawImage = new Image(_swapchain->imageFormat,
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    _swapchain->extent, VK_IMAGE_ASPECT_COLOR_BIT);

  Shader* defaultVert = new Shader(_device, "resources/shaders/defaultTriangleVertShader.spv", VK_SHADER_STAGE_VERTEX_BIT);
  Shader* defaultFrag = new Shader(_device, "resources/shaders/defaultTriangleFragShader.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

  defaultPipeline = new GraphicsPipeline(_device, _swapchain->imageFormat, {
    defaultVert->getStageCreateInfo(),
    defaultFrag->getStageCreateInfo()
    }, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  // --- Temporary Code ---

  //std::vector<Vertex> vertices(3);
  //vertices[0].position = { 0.25f, 0.25f, 0.0f };
  //vertices[1].position = { 0.75f, 0.25f, 0.0f };
  //vertices[2].position = { 0.50f, 0.75f, 0.0f };

  //std::vector<uint32_t> indices = { 0, 1, 2 };

  /*triangleBLAS = new BLAS(device, allocator, immediateSubmit, vertices, indices);

   worldTLAS = new TLAS(device, allocator, immediateSubmit, { triangleBLAS->getInstance(glm::mat4(1.0f)) });

   Shader* rayGen = new Shader(device, "resources/shaders/rayGenShader.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR);
   Shader* rayChit = new Shader(device, "resources/shaders/rayChitShader.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
   Shader* rayMiss = new Shader(device, "resources/shaders/rayMissShader.spv", VK_SHADER_STAGE_MISS_BIT_KHR);*/
}

GraphicsHandler::~GraphicsHandler() {
  _device->waitIdle();

  /*delete worldTLAS;
  delete triangleBLAS;*/

  delete _drawImage;
  delete _swapchain;

  delete _immediateSubmit;

  delete _allocator;
  delete _device;

#ifdef VKDEBUG
  delete _debugMessenger;
#endif // VKDEBUG

  delete _instance;
  delete _window;
}

void GraphicsHandler::Render(World* world) {
  beginDrawing();

  Frame* currentFrame = _swapchain->getCurrentFrame();

  std::vector<MeshRenderer*> renderers = world->getComponentsInChildren<MeshRenderer>();

  GraphicsPipeline* currentPipeline{ nullptr };

  // TODO: optimize the shit out of this piece of garbage
  for (auto renderer : renderers) {
    /*if (currentPipeline != renderer->material->pipeline) {
      currentPipeline = renderer->material->pipeline;
      currentFrame->commandBuffer->bindPipeline(renderer->material->pipeline);
    }*/
    currentFrame->commandBuffer->bindPipeline(renderer->material->pipeline);

    currentFrame->commandBuffer->pushConstants(renderer->getPushConstants(), renderer->material->pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT);

    currentFrame->commandBuffer->bindIndexBuffer(renderer->mesh->indices);

  }

  endDrawing();
}