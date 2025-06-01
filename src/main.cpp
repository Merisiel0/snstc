#define SDL_MAIN_HANDLED
#include "ECS/ECS.h"
#include "SDL3/SDL_main.h"
#include "events/EventHandler.h"
#include "graphics/vulkan/DescriptorManager.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "inputs/InputHandler.h"
#include "resources/Material.h"
#include "resources/Mesh.h"
#include "resources/ResourceManager.h"
#include "utils/Utils.h"

bool quit = false;

static void quitFunctor(SDL_QuitEvent e) { quit = true; }

static void quitFunctor(SDL_KeyboardEvent e) {
  if(e.key == SDLK_ESCAPE) { quit = true; }
}

struct ColdConstants {
  vec3 resolution;
  float time;
};

enum CustomGraphicsPipeline : uint16_t { CUSTOM_PIPELINE_COLD = 0 };

int main() {
  //--- Engine Initialization ---
  SDL_SetMainReady();
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD));

  ResourceManager::assetsPath = "../../src/assets";
  Time::init();
  InputHandler::init();

  EventHandler::quit += quitFunctor;
  EventHandler::keyDown += quitFunctor;

  try {
    VulkanHandler::init("SphereNSTC", 0, "SphereNSTC Engine", 0);
  } catch(std::runtime_error e) {
    std::cerr << "Failed to initialize Vulkan." << std::endl << e.what();
  }

  // --- Game Initialization ---
  World* world = new World();
  world->addTag("world");

  GameObject* cameraObj = new GameObject(world);
  cameraObj->addTag("camera");
  cameraObj->getComponent<Transform>()->position = {0, 0, 5};
  Camera& cam = cameraObj->addComponent<Camera>(PERSPECTIVE);
  cam.updateProjectionPerspective(70, 16.0f / 9.0f, 0.5f, 1000);
  cameraObj->addComponent<PlayerController>(3.0f, radians(40.f));

  GameObject* skyboxObj = new GameObject(world);
  skyboxObj->addComponent<Skybox>(
    std::vector<std::string> {ResourceManager::assetsPath + "/skyboxes/glsky/right.jpg",
      ResourceManager::assetsPath + "/skyboxes/glsky/left.jpg",
      ResourceManager::assetsPath + "/skyboxes/glsky/top.jpg",
      ResourceManager::assetsPath + "/skyboxes/glsky/bottom.jpg",
      ResourceManager::assetsPath + "/skyboxes/glsky/front.jpg",
      ResourceManager::assetsPath + "/skyboxes/glsky/back.jpg"});

  GameObject* planeObj = GameObject::createPrimitive(world, PLANE,
    ResourceManager::loadMaterial(ResourceManager::assetsPath + "/materials/checkered_wood_4k"));
  planeObj->addTag("checkered plane");
  planeObj->getComponent<Transform>()->rotate({radians(45.f), 0, 0}, WORLD);

  GameObject* planeObj2 =
    GameObject::createPrimitive(world, PLANE, ResourceManager::loadMaterial(Color {0, 1, 0, 1}));
  planeObj2->addTag("green lines plane");
  planeObj2->getComponent<Transform>()->rotate({radians(45.f), 0, 0}, WORLD);
  planeObj2->getComponent<Transform>()->translate({2, 0, 0}, WORLD);
  planeObj2->getComponent<MeshRenderer>()->setPolygonMode(VK_POLYGON_MODE_LINE);
  planeObj2->getComponent<MeshRenderer>()->lineWidth = 5;

  // plane with custom pipeline
  GameObject* planeObj3 = new GameObject(world);
  planeObj3->addTag("custom shader plane");
  planeObj3->getComponent<Transform>()->rotate({radians(45.f), 0, 0}, WORLD);
  planeObj3->getComponent<Transform>()->translate({-2, 0, 0}, WORLD);

  std::vector<VkPushConstantRange> ranges(1);
  ranges[0] = VulkanHandler::getDefaultPushConstantRange();
  // ranges[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  // ranges[1].offset = ranges[0].size;
  // ranges[1].size = sizeof(ColdConstants);

  std::vector<Shader> shaders;
  shaders.emplace_back(ResourceManager::assetsPath + "/shaders/vert0000.spv",
    VK_SHADER_STAGE_VERTEX_BIT);
  shaders.emplace_back(ResourceManager::assetsPath + "/shaders/cold.spv",
    VK_SHADER_STAGE_FRAGMENT_BIT);

  std::vector<DescriptorSetLayoutType> setLayouts = {DESCRIPTOR_SET_LAYOUT_GLOBAL};

  planeObj3->addComponent<MeshRenderer>(ResourceManager::generatePlane({1, 1}, {2, 2}),
    VK_CULL_MODE_BACK_BIT, CUSTOM_PIPELINE_COLD, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VK_POLYGON_MODE_FILL, ranges, shaders, setLayouts);

  // --- Game Loop ---
  while(!quit) {
    ResourceManager::cleanupExpired();
    EventHandler::processEvents();
    InputHandler::processInputs();

    Time::update();

    VulkanHandler::render(*world);
  }

  // --- Game Cleanup ---
  VulkanHandler::waitForEndOfWork();
  delete world;
  VulkanHandler::cleanup();
  SDL_Quit();

  return 0;
}
