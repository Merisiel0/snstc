#define SDL_MAIN_HANDLED
#include <filesystem>

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
  if (e.key == SDLK_ESCAPE) {
    quit = true;
  }
}

/**
 * @brief Cleans up engine after main exits
 *
 */

int main() {
  //--- Engine Initialization ---
  SDL_SetMainReady();
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD));

  Time::init();
  InputHandler::init();

  EventHandler::quit += quitFunctor;
  EventHandler::keyDown += quitFunctor;

  try {
    vulkanHandler = std::make_shared<VulkanHandler>("SphereNSTC", 0,
                                                    "SphereNSTC Engine", 0);
  } catch (std::runtime_error e) {
    std::cout << "Failed to initialize Vulkan." << std::endl << e.what();
  }

  // World world = World();
  // world.addTag("world");

  // // --- Game Initialization ---
  // GameObject cameraObj = GameObject(world);
  // cameraObj.addTag("camera");
  // cameraObj.getComponent<Transform>()->position = { 0,0,2 };
  // Camera& cam = cameraObj.addComponent<Camera>(PERSPECTIVE);
  // cam.updateProjectionPerspective(70, 16.0f / 9.0f, 0.5f, 1000);
  // cameraObj.addComponent<PlayerController>(3.0f, radians(40.f));

  // GameObject lightObj = GameObject(world);
  // lightObj.addTag("light");
  // lightObj.addComponent<AmbiantLight>(Color{ 1,1,1,0.2f });
  // lightObj.addComponent<Light>(Color{ 1,1,1,3 });
  // //lightObj.addComponent<MeshRenderer>(Mesh::generateIcoSphere(0, { 1,1,1,1
  // }), new Material()); lightObj.getComponent<Transform>()->scale = { .02f,
  // .02f, .02f };
  // //lightObj.addComponent<PlayerController>(50.0f, radians(40.f));

  // GameObject cubeObj = GameObject(world);
  // cubeObj.addTag("cube");
  // cubeObj.getComponent<Transform>()->position = { -5,0,0 };
  // cubeObj.getComponent<Transform>()->rotate(radians(45.f), radians(45.f), 0);
  // //cubeObj.addComponent<MeshRenderer>(Mesh::generateCube({ 1,1,0,.5f }), new
  // Material(), VK_CULL_MODE_FRONT_BIT);

  // GameObject planeObj = GameObject(world);
  // planeObj.addTag("plane");
  // planeObj.getComponent<Transform>()->rotate(radians(45.f), 0, 0);
  // // planeObj.addComponent<MeshRenderer>(Mesh::generatePlane({ 1,1 }, { 2,2
  // }),
  // // ResourceManager::loadMesh("../src/assets/materials/checkered_wood_4k"));

  // GameObject sphereObjUV = GameObject(world);
  // sphereObjUV.addTag("sphere");
  // sphereObjUV.getComponent<Transform>()->position = { 5,0,0 };
  // //sphereObjUV.addComponent<MeshRenderer>(Mesh::generateIcoSphere(0), new
  // Material(), VK_CULL_MODE_NONE, VK_POLYGON_MODE_LINE);

  // --- Game Loop ---
  // while (!quit) {
  //   EventHandler::processEvents();
  //   InputHandler::processInputs();

  //   Time::update();

  //   // vulkanHandler->render(world);
  // }

  // --- Game Cleanup ---
  vulkanHandler->waitForEndOfWork();
  SDL_Quit();

  return 0;
}