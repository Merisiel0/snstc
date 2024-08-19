#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define SDL_CHECK(x)												        																\
	do{																        																				\
		int err = x;												        																		\
		if (err != 0) {												        																	\
			std::cerr << "Detected SDL error: " << SDL_GetError() << std::endl;						\
		}															        																					\
	}while(0)

#define SDL_BCHECK(x)												        																\
	do{				    											        																			\
		SDL_bool err = x;											        																	\
		if (err == SDL_FALSE) {																													\
			std::cerr << "Detected SDL error: " << SDL_GetError() << std::endl;						\
		}															        																					\
	}while(0)

using namespace glm;

class Exception : public std::exception {
private:
	const char* message;

public:
	Exception(const char* msg) : message(msg) {}

	const char* what() {
		return message;
	}
};

typedef glm::uvec4 Color;

struct Vertex {
	Color color;
	glm::vec3 normal;
	glm::vec3 position;
	glm::vec2 uv;
};

static inline std::string readFileText(const char* path) {
	std::ifstream file(path, std::ios::ate | std::ios::in);

	if (!file.is_open()) {
		throw Exception("Failed to read file bytes.");
	}
	
	size_t fileSize = (size_t)file.tellg();
	std::string text;
	
	file.seekg(0);
	file.read(text.data(), fileSize);
	file.close();

	return text;
}

static inline std::vector<uint32_t> readFileBytes(const char* path) {
	std::ifstream file(path, std::ios::ate | std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		throw Exception("Failed to read file bytes.");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> fileBytes(fileSize / sizeof(uint32_t));

	file.seekg(0);
	file.read((char*)fileBytes.data(), fileSize);
	file.close();

	return fileBytes;
}