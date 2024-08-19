#pragma once

#include "SDL.h"
#include "glm/glm.hpp"
#include "Utils.h"
#include <vector>

struct GraphicsInitInfo {
	const char* applicationName{ nullptr };
	int applicationVersion{ 0 };
	const char* engineName{ nullptr };
	int engineVersion{ 0 };
};

enum WindowScreenMode {
	WINDOWED = 0,
	FULLSCREEN = SDL_WINDOW_FULLSCREEN,
	FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP
};

void initGraphics(GraphicsInitInfo initInfo);

void terminateGraphics();

struct MeshRenderer;

//void recordRenderer(MeshRenderer* renderer);

//void render();