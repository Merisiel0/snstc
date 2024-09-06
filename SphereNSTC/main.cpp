#define SDL_MAIN_HANDLED

#include "_utils/Utils.h"
#include "_events/EventHandler.h"
#include "_graphics/GraphicsHandler.h"
#include "_ECS/ECS.h"
#include "_resources/Mesh.h"
#include "_resources/Material.h"

#include <windows.h>

GraphicsHandler* graphicsHandler{ nullptr };
bool quit{ false };

int main() {
  // --- Engine Initialization ---
  SDL_SetMainReady();
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));

  GraphicsInitInfo initInfo{};
  initInfo.applicationName = "SphereNSTC";
  initInfo.applicationVersion = 0;
  initInfo.engineName = "SphereNSTC Engine";
  initInfo.engineVersion = 0;
  graphicsHandler = new GraphicsHandler(initInfo);

  // --- Game Initialization ---
  GameObject* cameraObj = new GameObject();
  cameraObj->addComponent<Transform>(vec3{ 0,0,-10 }, identity<quat>(), vec3{ 1,1,1 });
  Camera* cam = cameraObj->addComponent<Camera>(PERSPECTIVE);
  cam->updateProjectionPerspective(70, 16.0f / 9.0f, 0.5f, 1000);

  GameObject* cubeObj = new GameObject();
  cubeObj->addComponent<Transform>(vec3{ 0,0,0 }, identity<quat>(), vec3{ 1,1,1 });
  cubeObj->addComponent<MeshRenderer>(Mesh::generateCube(), Material::load(graphicsHandler->defaultPipeline, "checkered_wood_4k"));

  World* world = new World({ cameraObj, cubeObj });

  // --- Game Loop ---
  while (!quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
        break;
      }
    }

    graphicsHandler->Render(world);
  }

  // --- Engine Cleanup ---
  delete world;
  delete graphicsHandler;
  SDL_Quit();

  return 0;
}


/*
[ VUID-VkImageMemoryBarrier2-oldLayout-01197 ]
Object 0: handle = 0x29d2258e1a0, type = VK_OBJECT_TYPE_COMMAND_BUFFER;
Object 1: handle = 0x967dd1000000000e, type = VK_OBJECT_TYPE_IMAGE;
vkCmdPipelineBarrier2():
pDependencyInfo->pImageMemoryBarriers[0].image cannot transition the layout of aspect=1, level=0, layer=0 from
VK_IMAGE_LAYOUT_PRESENT_SRC_KHR when the previously used layout is VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
The Vulkan spec states: If srcQueueFamilyIndex and dstQueueFamilyIndex define a queue family ownership transfer or
oldLayout and newLayout define an image layout transition, oldLayout must be VK_IMAGE_LAYOUT_UNDEFINED or
the current layout of the image subresources affected by the barrier
(https://vulkan.lunarg.com/doc/view/1.3.275.0/windows/1.3-extensions/vkspec.html#VUID-VkImageMemoryBarrier2-oldLayout-01197)

[ UNASSIGNED-CoreValidation-DrawState-InvalidImageLayout ]
Object 0: handle = 0x29d2258e1a0, type = VK_OBJECT_TYPE_COMMAND_BUFFER;
Object 1: handle = 0x967dd1000000000e, type = VK_OBJECT_TYPE_IMAGE;
vkQueueSubmit2(): pSubmits[0].pCommandBufferInfos[0].commandBuffer command buffer
expects image (subresource: aspectMask 0x1 array layer 0, mip level 0) to be in layout
VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL--instead, current layout is VK_IMAGE_LAYOUT_UNDEFINED.

[ UNASSIGNED-CoreValidation-DrawState-InvalidImageLayout ]
Object 0: handle = 0x29d2258e1a0, type = VK_OBJECT_TYPE_COMMAND_BUFFER;
Object 1: handle = 0xdcc8fd0000000012, type = VK_OBJECT_TYPE_IMAGE;
vkQueueSubmit2():
pSubmits[0].pCommandBufferInfos[0].commandBuffer command buffer
expects image (subresource: aspectMask 0x1 array layer 0, mip level 0) to be in layout
VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL--instead, current layout is VK_IMAGE_LAYOUT_UNDEFINED.

[ VUID-VkPresentInfoKHR-pImageIndices-01430 ]
Object 0: handle = 0x29d226986a0, type = VK_OBJECT_TYPE_QUEUE;
vkQueuePresentKHR():
pPresentInfo->pSwapchains[0] images passed to present must be in layout VK_IMAGE_LAYOUT_PRESENT_SRC_KHR or
VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR but is in VK_IMAGE_LAYOUT_UNDEFINED.
The Vulkan spec states: Each element of pImageIndices must be the index of a presentable image acquired from the swapchain specified by the corresponding element of the pSwapchains array, and the presented image subresource must be in the VK_IMAGE_LAYOUT_PRESENT_SRC_KHR or VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR layout at the time the operation is executed on a VkDevice (https://vulkan.lunarg.com/doc/view/1.3.275.0/windows/1.3-extensions/vkspec.html#VUID-VkPresentInfoKHR-pImageIndices-01430)

*/