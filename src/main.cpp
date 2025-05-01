#define SDL_MAIN_HANDLED
#include "ECS/ECS.h"
#include "SDL3/SDL_main.h"
#include "events/EventHandler.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "inputs/InputHandler.h"
#include "resources/Material.h"
#include "resources/Mesh.h"
#include "resources/ResourceManager.h"
#include "utils/Utils.h"

std::shared_ptr<VulkanHandler> vulkanHandler;
bool quit = false;

static void quitFunctor(SDL_QuitEvent e) { quit = true; }

static void quitFunctor(SDL_KeyboardEvent e) {
  if(e.key == SDLK_ESCAPE) { quit = true; }
}

int main() {
  //--- Engine Initialization ---
  SDL_SetMainReady();
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD));

  ResourceManager::assetsPath = "../..";
  Time::init();
  InputHandler::init();

  EventHandler::quit += quitFunctor;
  EventHandler::keyDown += quitFunctor;

  try {
    vulkanHandler = std::make_shared<VulkanHandler>("SphereNSTC", 0, "SphereNSTC Engine", 0);
  } catch(std::runtime_error e) {
    std::cerr << "Failed to initialize Vulkan." << std::endl << e.what();
  }

  World world = World();
  world.addTag("world");

  // --- Game Initialization ---
  GameObject cameraObj = GameObject(world);
  cameraObj.addTag("camera");
  cameraObj.getComponent<Transform>()->position = {0, 0, 5};
  Camera& cam = cameraObj.addComponent<Camera>(PERSPECTIVE);
  cam.updateProjectionPerspective(70, 16.0f / 9.0f, 0.5f, 1000);

  GameObject skyboxObj = GameObject(world);
  skyboxObj.addComponent<Skybox>(
    std::vector<std::string> {ResourceManager::assetsPath + "/src/assets/skyboxes/glsky/right.jpg",
      ResourceManager::assetsPath + "/src/assets/skyboxes/glsky/left.jpg",
      ResourceManager::assetsPath + "/src/assets/skyboxes/glsky/top.jpg",
      ResourceManager::assetsPath + "/src/assets/skyboxes/glsky/bottom.jpg",
      ResourceManager::assetsPath + "/src/assets/skyboxes/glsky/front.jpg",
      ResourceManager::assetsPath + "/src/assets/skyboxes/glsky/back.jpg"});
  skyboxObj.addComponent<PlayerController>(3.0f, radians(40.f));

  // GameObject cubeObj = GameObject::createPrimitive(world, CUBE);
  // cubeObj.addTag("cube");

  // GameObject lightObj = GameObject(world);
  // lightObj.addComponent<MeshRenderer>(ResourceManager::generateIcoSphere(1), ResourceManager::loadMaterial(Color{0, 0, 1, 1}));
  // lightObj.addComponent<AmbiantLight>(Color {1, 1, 1, 0.2f});
  // lightObj.addComponent<Light>(Color {1, 1, 1, 3});
  // lightObj.addComponent<PlayerController>(3.0f, radians(40.f));

  // --- Game Loop ---
  int frames = 0;
  while(!quit) {
    ResourceManager::cleanupResources();
    EventHandler::processEvents();
    InputHandler::processInputs();

    Time::update();

    vulkanHandler->render(world);

    frames++;
    if(frames == 1) {
      //quit = true;
    }
  }

  // --- Game Cleanup ---
  vulkanHandler->waitForEndOfWork();
  SDL_Quit();

  return 0;
}