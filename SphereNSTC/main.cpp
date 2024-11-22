// [---{ TODO LIST }---]
/*
* --- ECS ---
* 1. GameObject constructor not called on delete world?!?
* 2. make each world have its own registry (permits use of views)
*
* --- renderer ---
* 1. implement all other prb maps
* 2. implement device feature checks
* 3. abort crash on close -> some resources aren't deallocated
* 4. implement lights
*
* --- inputs ---
* 1. fix rotations
* 2. gamepad controls
*
*/


#define SDL_MAIN_HANDLED
#include "SDL3/SDL_main.h"

#include "_utils/Utils.h"
#include "_events/EventHandler.h"
#include "_inputs/InputHandler.h"
#include "_graphics/_vulkan/VulkanHandler.h"
#include "_ECS/ECS.h"
#include "_resources/Mesh.h"
#include "_resources/Material.h"

VulkanHandler* vulkanHandler{ nullptr };
bool quit = false;
static void quitFunctor(SDL_QuitEvent e) { quit = true; }
static void quitFunctor(SDL_KeyboardEvent e) { if (e.key == SDLK_ESCAPE) quit = true; }

int main() {
  // --- Engine Initialization ---
  SDL_SetMainReady();
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD));
  Time::init();
  InputHandler::init();
  vulkanHandler = new VulkanHandler("SphereNSTC", 0, "SphereNSTC Engine", 0);

  EventHandler::quit += quitFunctor;
  EventHandler::keyDown += quitFunctor;

  // --- Game Initialization ---
  GameObject* cameraObj = new GameObject();
  cameraObj->getComponent<Transform>()->position = { 0,0,2 };
  Camera* cam = cameraObj->addComponent<Camera>(PERSPECTIVE);
  cam->updateProjectionPerspective(70, 16.0f / 9.0f, 0.5f, 1000);
  //cameraObj->addComponent<PlayerController>(3.0f, radians(40.f));

  GameObject* lightObj = new GameObject();
  lightObj->addComponent<AmbiantLight>(Color{ 1,1,1,0.2f });
  lightObj->addComponent<Light>(Color{ 1,1,1,3 });
  lightObj->addComponent<MeshRenderer>(Mesh::generateIcoSphere(0, { 1,1,1,1 }), new Material());
  lightObj->getComponent<Transform>()->scale = { .02f, .02f, .02f };
  lightObj->addComponent<PlayerController>(50.0f, radians(40.f));

  GameObject* cubeObj = new GameObject();
  //cubeObj->getComponent<Transform>()->position = { -5,0,0 };
  cubeObj->getComponent<Transform>()->rotate(radians(45.f), radians(45.f), 0);
  cubeObj->addComponent<MeshRenderer>(Mesh::generateCube({ 1,1,0,.5f }), new Material(), VK_CULL_MODE_FRONT_BIT);

  GameObject* planeObj = new GameObject();
  planeObj->getComponent<Transform>()->position = { -5,0,0 };
  planeObj->getComponent<Transform>()->rotate(radians(45.f), 0, 0);
  planeObj->addComponent<MeshRenderer>(Mesh::generatePlane({ 1,1 }, { 2,2 }),
    Material::load("resources/materials/checkered_wood_4k"));

  GameObject* sphereObjUV = new GameObject();
  sphereObjUV->getComponent<Transform>()->position = { 5,0,0 };
  sphereObjUV->addComponent<MeshRenderer>(Mesh::generateIcoSphere(0), new Material(), VK_CULL_MODE_NONE, VK_POLYGON_MODE_LINE);

  World* world = new World({ cameraObj, lightObj, planeObj });

  // --- Game Loop ---
  while (!quit) {
    EventHandler::processEvents();
    InputHandler::processInputs();

    Time::update();

    vulkanHandler->render(world);
  }

  // --- Game Cleanup ---
  vulkanHandler->waitForEndOfWork();
  delete world; // todo: bug when deleter gets called

  // --- Engine Cleanup ---
  delete vulkanHandler;
  SDL_Quit();

  return 0;
}