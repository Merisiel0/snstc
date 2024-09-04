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
	GameObject cameraObj = GameObject();
	cameraObj.addComponent<Transform>(vec3{ 0,0,-10 }, identity<quat>(), vec3{ 1,1,1 });
	Camera* cam = cameraObj.addComponent<Camera>(PERSPECTIVE);
	cam->updateProjectionPerspective(70, 16.0f / 9.0f, 0.5f, 1000);

	GameObject cubeObj = GameObject();
	cubeObj.addComponent<Transform>(vec3{ 0,0,0 }, identity<quat>(), vec3{ 1,1,1 });
	cubeObj.addComponent<MeshRenderer>(Mesh::generateCube(), Material::load(graphicsHandler->defaultPipeline, "checkered_wood_4k"));

	World world = World({cameraObj, cubeObj});

	// --- Game Loop ---
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
			}
		}

		graphicsHandler->Render(&world);
	}

	// --- Engine Cleanup ---
	delete graphicsHandler;
	SDL_Quit();

	return 0;
}